# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

bl_info = {
    "name": "Export Quick3D Format(.q3d)",
    "author": "Guillaume Darier",
    "version": (1, 3, 0),
    "blender": (3, 1, 0),
    "location": "File > Export > Quick3D (.q3d)",
    "description": "Export mesh Quick3D data (.q3d)",
    "warning": "",
    "wiki_url": "http://"
                "",
    "category": "Import-Export"}

import bpy
from bpy.props import BoolProperty, IntProperty, EnumProperty
import mathutils
from bpy_extras.io_utils import ExportHelper
import bpy_extras.io_utils
import os
import time
import math
import struct

#--------------------------------------------------------------------------------------------------

# Object types the exporter can translate to Quick3D components. Cameras, lights and
# other scene helpers have no Quick3D representation (their data blocks expose neither
# vertices nor materials) and are skipped instead of crashing the export.
EXPORTABLE_OBJECT_TYPES = ('MESH', 'ARMATURE')

# Maximum number of XML lines buffered in memory before a single disk write.
# Buffering avoids one file.write() system call per vertex/face, which dominates
# export time on dense meshes once the algorithmic hot spots are fixed.
EXPORT_CHUNK_LINE_COUNT = 4096

# Target number of progress bar updates for a whole export. Updates are throttled
# to this count so huge meshes do not flood the UI with redraw requests.
EXPORT_PROGRESS_UPDATE_COUNT = 200

#--------------------------------------------------------------------------------------------------

def writeLine(file, text):
    file.write('\t' * file.indent + text + '\n')

#--------------------------------------------------------------------------------------------------

def openNode(file, text, singleLine, **attributes):
    final = '<' + text;

    for key, value in attributes.items():
        final = final + ' {0}="{1}"'.format(key, value)

    if (singleLine):
        final = final + '/>'
    else:
        final = final + '>'

    writeLine(file, final)

    if (singleLine == 0):
        file.indent = file.indent + 1

#--------------------------------------------------------------------------------------------------

def closeNode(file, text):
    file.indent = file.indent - 1
    writeLine(file, '</' + text + '>')

#--------------------------------------------------------------------------------------------------

def principled_bsdf(mat):
    if mat.use_nodes == False:
        return None

    for node in mat.node_tree.nodes:
        if node.type == 'BSDF_PRINCIPLED':
            return node

    return None

def writeMaterial(file, mat, filepath):
    source_dir = os.path.dirname(bpy.data.filepath)

    openNode(file, 'Material', 0, Name = mat.name)

    diffuse_map = ''
    diffuse_color = mat.diffuse_color[:3] if hasattr(mat, 'diffuse_color') else (1.0, 1.0, 1.0)
    specular_color = (1.0, 1.0, 1.0)

    bsdf = principled_bsdf(mat)

    if bsdf is not None:
        diffuse_color = bsdf.inputs['Base Color'].default_value[:3]
        specular_value = bsdf.inputs['Specular'].default_value
        specular_color = (specular_value, specular_value, specular_value)

        for link in bsdf.inputs['Base Color'].links:
            if link.from_node.type == 'TEX_IMAGE' and link.from_node.image is not None:
                head, tail = os.path.split(link.from_node.image.filepath)
                diffuse_map = 'Textures/' + tail
                break

    openNode(file, 'Diffuse', 1,
        r = '{0:.4f}'.format(diffuse_color[0]),
        g = '{0:.4f}'.format(diffuse_color[1]),
        b = '{0:.4f}'.format(diffuse_color[2]),
        Map = diffuse_map
        )

    openNode(file, 'Specular', 1,
        r = '{0:.4f}'.format(specular_color[0]),
        g = '{0:.4f}'.format(specular_color[1]),
        b = '{0:.4f}'.format(specular_color[2])
        )

    closeNode(file, 'Material')

#--------------------------------------------------------------------------------------------------

def writeMaterials(file, materials, filepath, progress):
    for mat in materials:
        writeMaterial(file, mat, filepath)
        progress.advance(1)

#--------------------------------------------------------------------------------------------------

def getVertexUVs(mesh):
    # Builds one UV coordinate per vertex in a single pass over the mesh loops.
    # UV layers are stored per loop in Blender: the previous implementation scanned every
    # face for every vertex, which is quadratic and dominates export time on dense meshes.
    # When a vertex is shared by several loops (texture seams), the first loop UV is kept.
    if mesh.uv_layers.active is None:
        return []

    vertex_count = len(mesh.vertices)
    uvs = [None] * vertex_count
    uv_data = mesh.uv_layers.active.data
    missing_count = vertex_count

    for loop in mesh.loops:
        vertex_index = loop.vertex_index
        if uvs[vertex_index] is None:
            uvs[vertex_index] = uv_data[loop.index].uv
            missing_count = missing_count - 1
            if missing_count == 0:
                break

    return uvs

#--------------------------------------------------------------------------------------------------

def writeMesh(file, object, depsgraph, material_index_by_id, scene, props, progress):
    if props.apply_modifiers:
        mesh = object.evaluated_get(depsgraph).data
    else:
        mesh = object.data

    if mesh is None:
        return

    if props.world_space:
        mesh.transform(object.matrix_world)

    openNode(file, 'Component', 0, Name=object.name, Class="CMesh")

    pos = object.matrix_local.to_translation()
    rot = object.matrix_local.to_euler('ZXY')

    openNode(file, 'Position', 1,
        x = '{0:.4f}'.format(pos[0]),
        y = '{0:.4f}'.format(pos[2]),
        z = '{0:.4f}'.format(pos[1])
        )

    openNode(file, 'Rotation', 1,
        x = '{0:.4f}'.format(-rot[0]),
        y = '{0:.4f}'.format(rot[2]),
        z = '{0:.4f}'.format(rot[1])
        )

    openNode(file, 'Vertices', 0)

    # Write vertices in buffered chunks: one file.write() per chunk instead of one per vertex.
    uvs = getVertexUVs(mesh)
    has_uvs = len(uvs) > 0
    vertex_indent = '\t' * file.indent
    vertex_lines = []

    for vertex in mesh.vertices:
        vertex_id = vertex.index
        coord = vertex.co
        vertex_u = 0.0
        vertex_v = 0.0

        if has_uvs and vertex_id < len(uvs):
            uv = uvs[vertex_id]
            if uv is not None:
                vertex_u = uv[0]
                vertex_v = uv[1]

        vertex_lines.append(
            '{0}<Vertex x="{1:.4f}" y="{2:.4f}" z="{3:.4f}" u="{4:.4f}" v="{5:.4f}"/>\n'.format(
                vertex_indent, coord[0], coord[2], coord[1], vertex_u, vertex_v))

        if len(vertex_lines) >= EXPORT_CHUNK_LINE_COUNT:
            file.write(''.join(vertex_lines))
            vertex_lines = []
            progress.advance(EXPORT_CHUNK_LINE_COUNT)

    if len(vertex_lines) > 0:
        progress.advance(len(vertex_lines))
        file.write(''.join(vertex_lines))

    closeNode(file, 'Vertices')

    openNode(file, 'Faces', 0)

    # Write faces in buffered chunks. The material lookup table is built once per export
    # instead of scanning the material list for every face.
    face_indent = '\t' * file.indent
    face_lines = []
    face_materials = mesh.materials

    for face in mesh.polygons:
        vertlist = ', '.join(str(vertex_index) for vertex_index in face.vertices)

        try:
            local_material = face_materials[face.material_index]
            if local_material is None:
                global_material_index = 0
            else:
                global_material_index = material_index_by_id.get(id(local_material), 0)
        except IndexError:
            global_material_index = 0

        face_lines.append('{0}<Face Vertices="{1}" Material="{2}"/>\n'.format(
            face_indent, vertlist, global_material_index))

        if len(face_lines) >= EXPORT_CHUNK_LINE_COUNT:
            file.write(''.join(face_lines))
            face_lines = []
            progress.advance(EXPORT_CHUNK_LINE_COUNT)

    if len(face_lines) > 0:
        progress.advance(len(face_lines))
        file.write(''.join(face_lines))

    closeNode(file, 'Faces')

    # Write vertex groups in a single pass over the vertices. The previous implementation
    # called group.weight() once per (group, vertex) pair and concatenated the weight string
    # incrementally, which is slow and quadratic. It also never reset its cursors between
    # groups, so every group after the first one was exported with wrong weights.
    vertex_groups = object.vertex_groups
    vertex_count = len(mesh.vertices)

    if len(vertex_groups) > 0 and vertex_count > 0:
        weights_by_group_index = {}

        for vertex_group in vertex_groups:
            weights_by_group_index[vertex_group.index] = [0.0] * vertex_count

        for vertex in mesh.vertices:
            for group_element in vertex.groups:
                column = weights_by_group_index.get(group_element.group)
                if column is not None:
                    column[vertex.index] = group_element.weight

        for vertex_group in vertex_groups:
            weight_list = ', '.join('{0:.4f}'.format(weight) for weight in weights_by_group_index[vertex_group.index])
            openNode(file, 'VertexGroup', 1, Name=vertex_group.name, Weights=weight_list)
            progress.advance(vertex_count)

    # Write child objects
    for child in object.children:
        writeSingleObject(file, child, depsgraph, material_index_by_id, scene, props, progress)

    closeNode(file, 'Component')

#--------------------------------------------------------------------------------------------------

def writeBone(file, bone, progress):
    openNode(file, 'Component', 0, Name=bone.name, Class='CBone')

    # Write head
    head_x = bone.head[0]
    head_y = bone.head[2]
    head_z = bone.head[1]

    openNode(file, 'Head', 1,
        x = '{0:.4f}'.format(head_x),
        y = '{0:.4f}'.format(head_y),
        z = '{0:.4f}'.format(head_z)
        )

    # Write tail
    tail_x = bone.tail[0]
    tail_y = bone.tail[2]
    tail_z = bone.tail[1]

    openNode(file, 'Tail', 1,
        x = '{0:.4f}'.format(tail_x),
        y = '{0:.4f}'.format(tail_y),
        z = '{0:.4f}'.format(tail_z)
        )

    # Write X axis
    axis_x = bone.x_axis[0]
    axis_y = bone.x_axis[2]
    axis_z = bone.x_axis[1]

    openNode(file, 'XAxis', 1,
        x = '{0:.4f}'.format(axis_x),
        y = '{0:.4f}'.format(axis_y),
        z = '{0:.4f}'.format(axis_z)
        )

    # Write Y axis
    axis_x = bone.z_axis[0]
    axis_y = bone.z_axis[2]
    axis_z = bone.z_axis[1]

    openNode(file, 'YAxis', 1,
        x = '{0:.4f}'.format(axis_x),
        y = '{0:.4f}'.format(axis_y),
        z = '{0:.4f}'.format(axis_z)
        )

    # Write Z axis
    axis_x = bone.y_axis[0]
    axis_y = bone.y_axis[2]
    axis_z = bone.y_axis[1]

    openNode(file, 'ZAxis', 1,
        x = '{0:.4f}'.format(axis_x),
        y = '{0:.4f}'.format(axis_y),
        z = '{0:.4f}'.format(axis_z)
        )

    for child in bone.children:
        writeBone(file, child, progress)

    closeNode(file, 'Component')
    progress.advance(1)

#--------------------------------------------------------------------------------------------------

def writeArmature(file, object, depsgraph, material_index_by_id, scene, props, progress):
    openNode(file, 'Component', 0, Name=object.name, Class="CArmature")

    armature = object.data

    for bone in armature.bones:
        if bone.parent is None:
            writeBone(file, bone, progress)

    # Write child objects
    for child in object.children:
        writeSingleObject(file, child, depsgraph, material_index_by_id, scene, props, progress)

    closeNode(file, 'Component')

#--------------------------------------------------------------------------------------------------

def writeSingleObject(file, object, depsgraph, material_index_by_id, scene, props, progress):
    if object.type == 'ARMATURE':
        writeArmature(file, object, depsgraph, material_index_by_id, scene, props, progress)
        progress.advance(1)
    elif object.type == 'MESH':
        writeMesh(file, object, depsgraph, material_index_by_id, scene, props, progress)
        progress.advance(1)
    else:
        # Other object types (cameras, lights, ...) have no Quick3D representation:
        # skip them but still visit their children.
        for child in object.children:
            writeSingleObject(file, child, depsgraph, material_index_by_id, scene, props, progress)

#--------------------------------------------------------------------------------------------------

def writeTopLevelObjects(file, objects, depsgraph, material_index_by_id, scene, props, progress):
    for object in objects:
        if object.parent is None:
            writeSingleObject(file, object, depsgraph, material_index_by_id, scene, props, progress)

#--------------------------------------------------------------------------------------------------

def getMaterials(objects, depsgraph, props):
    materials = []

    for object in objects:

        if object.type != 'MESH':
            continue

        if props.apply_modifiers:
            mesh = object.evaluated_get(depsgraph).data
        else:
            mesh = object.data

        if mesh is not None:
            for mat in mesh.materials:
                try:
                    i = materials.index(mat)
                except ValueError:
                    materials.append(mat)

    return materials

#--------------------------------------------------------------------------------------------------

def get_sampled_frames(start, end, sampling):
    return [math.modf(start + x * sampling) for x in range(int((end - start) / sampling) + 1)]

#--------------------------------------------------------------------------------------------------

class Q3DExportProgress:
    # Forwards export progress to the Blender status bar. Updates are throttled so huge
    # meshes do not flood the UI with redraw requests. The window manager is optional
    # so exports can also run headless (unit tests, background mode).

    def __init__(self, window_manager, total_steps):
        self.window_manager = window_manager
        self.total_steps = max(1, int(total_steps))
        self.done_steps = 0
        self.steps_since_update = 0
        self.update_interval = max(1, self.total_steps // EXPORT_PROGRESS_UPDATE_COUNT + 1)

        if self.window_manager is not None:
            self.window_manager.progress_begin(0, self.total_steps)

    def advance(self, steps):
        if steps <= 0:
            return

        self.done_steps = self.done_steps + steps
        self.steps_since_update = self.steps_since_update + steps

        if self.window_manager is not None and self.steps_since_update >= self.update_interval:
            self.window_manager.progress_update(min(self.done_steps, self.total_steps))
            self.steps_since_update = 0

    def finish(self):
        if self.window_manager is not None:
            self.window_manager.progress_update(self.total_steps)
            self.window_manager.progress_end()

#--------------------------------------------------------------------------------------------------

def countExportSteps(object):
    # Estimates the progress steps of one object subtree from the original (non evaluated)
    # mesh sizes. Evaluated meshes may differ when modifiers are applied; the progress
    # bar clamps to its maximum in that case. Non exportable objects count zero for
    # themselves but their children are still visited.
    if object.type not in EXPORTABLE_OBJECT_TYPES:
        steps = 0
    else:
        steps = 1

        mesh_data = getattr(object, 'data', None)
        vertices = getattr(mesh_data, 'vertices', None)
        polygons = getattr(mesh_data, 'polygons', None)
        vertex_count = len(vertices) if vertices is not None else 0
        face_count = len(polygons) if polygons is not None else 0
        steps = steps + vertex_count + face_count

        try:
            steps = steps + vertex_count * len(object.vertex_groups)
        except TypeError:
            pass

    for child in getattr(object, 'children', ()):
        steps = steps + countExportSteps(child)

    return steps

#--------------------------------------------------------------------------------------------------

def buildMaterialIndex(materials):
    # Builds the material lookup table once per export instead of scanning the material
    # list for every face. Materials are keyed by id() because RNA structs are looked up
    # by identity here and the materials list keeps them alive during the whole export.
    material_index_by_id = {}

    for material_index, material in enumerate(materials):
        material_index_by_id[id(material)] = material_index

    return material_index_by_id

#--------------------------------------------------------------------------------------------------

def do_export(context, props, filepath, window_manager=None):
    scene = context.scene
    depsgraph = context.evaluated_depsgraph_get()
    start = props.range_start
    end = props.range_end
    sampling = float(props.sampling)

    objects = scene.objects

    file = open(filepath, "wt")
    file.indent = 0

    total_steps = 0

    for object in objects:
        if object.parent is None:
            total_steps = total_steps + countExportSteps(object)

    progress = Q3DExportProgress(window_manager, total_steps)

    try:
        # Write XML Header
        writeLine(file, '<?xml version="1.0" encoding="UTF-8"?>')

        openNode(file, 'Root', 0)

        materials = getMaterials(objects, depsgraph, props)

        writeMaterials(file, materials, filepath, progress)

        material_index_by_id = buildMaterialIndex(materials)

        writeTopLevelObjects(file, objects, depsgraph, material_index_by_id, scene, props, progress)

        closeNode(file, 'Root')

        file.flush()
    finally:
        progress.finish()
        file.close()

    return True

#--------------------------------------------------------------------------------------------------

###### EXPORT OPERATOR #######
class Export_q3d(bpy.types.Operator, ExportHelper):
    """Export the active Object as a .q3d file"""
    bl_idname = "export_shape.q3d"
    bl_label = "Export Quick3D (.q3d)"

    filename_ext = ".q3d"

    rot_x90: BoolProperty(name="Convert to Y-up",
            description="Rotate 90 degrees around X to convert to y-up",
            default=True,
            )
    world_space: BoolProperty(name="Export into Worldspace",
            description="Transform the Vertexcoordinates into Worldspace",
            default=False,
            )
    apply_modifiers: BoolProperty(name="Apply Modifiers",
            description="Applies the Modifiers",
            default=True,
            )
    range_start: IntProperty(name='Start Frame',
            description='First frame to use for Export',
            default=1,
            )
    range_end: IntProperty(name='End Frame',
            description='Last frame to use for Export',
            default=250,
            )
    sampling: EnumProperty(name='Sampling',
            description='Sampling --> frames per sample (0.1 yields 10 samples per frame)',
            items=(('0.01', '0.01', ''),
                   ('0.05', '0.05', ''),
                   ('0.1', '0.1', ''),
                   ('0.2', '0.2', ''),
                   ('0.25', '0.25', ''),
                   ('0.5', '0.5', ''),
                   ('1', '1', ''),
                   ('2', '2', ''),
                   ('3', '3', ''),
                   ('4', '4', ''),
                   ('5', '5', ''),
                   ('10', '10', ''),
                   ),
            default='1',
            )

    @classmethod
    def poll(cls, context):
        return context.active_object is not None and context.active_object.type in {'MESH', 'CURVE', 'SURFACE', 'FONT', 'ARMATURE'}

    def execute(self, context):
        start_time = time.time()
        print('\n_____START_____')
        props = self.properties
        filepath = self.filepath
        filepath = bpy.path.ensure_ext(filepath, self.filename_ext)

        exported = do_export(context, props, filepath, context.window_manager)

        if exported:
            elapsed = time.time() - start_time
            print('finished export in %s seconds' %(elapsed))
            print(filepath)
            self.report({'INFO'}, 'Quick3D export finished in %.1f seconds' %(elapsed))

        return {'FINISHED'}

    def invoke(self, context, event):
        wm = context.window_manager
        wm.fileselect_add(self)
        return {'RUNNING_MODAL'}

### REGISTER ###

def menu_func(self, context):
    self.layout.operator(Export_q3d.bl_idname, text="Quick3D (.q3d)")

def register():
    bpy.utils.register_class(Export_q3d)

    bpy.types.TOPBAR_MT_file_export.append(menu_func)
    #bpy.types.VIEW3D_PT_tools_objectmode.prepend(menu_func)

def unregister():
    bpy.utils.unregister_class(Export_q3d)

    bpy.types.TOPBAR_MT_file_export.remove(menu_func)
    #bpy.types.VIEW3D_PT_tools_objectmode.remove(menu_func)

if __name__ == "__main__":
    register()
