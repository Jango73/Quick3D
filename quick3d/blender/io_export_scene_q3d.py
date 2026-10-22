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
    "version": (1, 2, 0),
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

def writeLine(file, text):
    pretext=''

    for index in range(file.indent):
        pretext = pretext + '\t'

    file.write(pretext + text + '\n')

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

def writeMaterials(file, materials, filepath):
    for mat in materials:
        writeMaterial(file, mat, filepath)

#--------------------------------------------------------------------------------------------------

def getVertexUVs(mesh):
    uvs = []

    if mesh.uv_layers.active is None:
        return uvs

    vertex_index = 0
    uv_layer = mesh.uv_layers.active.data[:]

    for vertex in mesh.vertices:
        for face in mesh.polygons:
            for loop_index in face.loop_indices:
                if mesh.loops[loop_index].vertex_index == vertex_index:
                    uvs.append(uv_layer[loop_index].uv)

        vertex_index = vertex_index + 1

    return uvs

#--------------------------------------------------------------------------------------------------

def writeMesh(file, object, depsgraph, materials, scene, props):
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

    # Write vertices
    vertex_index = 0
    uvs = getVertexUVs(mesh)

    for vertex in mesh.vertices:
        vert_x = vertex.co[0]
        vert_y = vertex.co[2]
        vert_z = vertex.co[1]
        vert_u = 0.0
        vert_v = 0.0

        if vertex_index < len(uvs):
            vert_u = uvs[vertex_index].x
            vert_v = uvs[vertex_index].y

        openNode(file, 'Vertex', 1,
            x = '{0:.4f}'.format(vert_x),
            y = '{0:.4f}'.format(vert_y),
            z = '{0:.4f}'.format(vert_z),
            u = '{0:.4f}'.format(vert_u),
            v = '{0:.4f}'.format(vert_v)
            )

        vertex_index = vertex_index + 1

    closeNode(file, 'Vertices')

    openNode(file, 'Faces', 0)

    # Write faces
    for face in mesh.polygons:

        # Vertices
        vertlist = ''

        for vertex in face.vertices:
            if (len(vertlist) > 0):
                vertlist = vertlist + ', '
            vertlist = vertlist + str(vertex)

        # Material
        try:
            local_mat = mesh.materials[face.material_index]
            global_mat_index = materials.index(local_mat)
        except IndexError:
            global_mat_index = 0

        openNode(file, 'Face', 1, Vertices=vertlist, Material=global_mat_index)

    closeNode(file, 'Faces')

    # Write vertex groups
    vertex_index = 0
    weight = 0
    weight_list = ''

    for group in object.vertex_groups:

        for vertex in mesh.vertices:
            try:
                weight = group.weight(vertex_index)
            except:
                weight = 0.0

            if (len(weight_list) > 0):
                weight_list = weight_list + ', '
            weight_list = weight_list + "{0:.4f}".format(weight)
            vertex_index = vertex_index + 1

        openNode(file, 'VertexGroup', 1, Name=group.name, Weights=weight_list)

    # Write child objects
    for child in object.children:
        writeSingleObject(file, child, depsgraph, materials, scene, props)

    closeNode(file, 'Component')

#--------------------------------------------------------------------------------------------------

def writeBone(file, bone, materials, scene, props):
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
        writeBone(file, child, materials, scene, props)

    closeNode(file, 'Component')

#--------------------------------------------------------------------------------------------------

def writeArmature(file, object, depsgraph, materials, scene, props):
    openNode(file, 'Component', 0, Name=object.name, Class="CArmature")

    armature = object.data

    for bone in armature.bones:
        if bone.parent is None:
            writeBone(file, bone, materials, scene, props)

    # Write child objects
    for child in object.children:
        writeSingleObject(file, child, depsgraph, materials, scene, props)

    closeNode(file, 'Component')

#--------------------------------------------------------------------------------------------------

def writeSingleObject(file, object, depsgraph, materials, scene, props):
    if object.type == 'ARMATURE':
        writeArmature(file, object, depsgraph, materials, scene, props)
    else:
        writeMesh(file, object, depsgraph, materials, scene, props)

#--------------------------------------------------------------------------------------------------

def writeTopLevelObjects(file, objects, depsgraph, materials, scene, props):
    for object in objects:
        if object.parent is None:
            writeSingleObject(file, object, depsgraph, materials, scene, props)

#--------------------------------------------------------------------------------------------------

def getMaterials(objects, depsgraph, props):
    materials = []

    for object in objects:

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

def do_export(context, props, filepath):
    scene = context.scene
    depsgraph = context.evaluated_depsgraph_get()
    start = props.range_start
    end = props.range_end
    sampling = float(props.sampling)

    objects = scene.objects

    file = open(filepath, "wt")
    file.indent = 0

    # Write XML Header
    writeLine(file, '<?xml version="1.0" encoding="UTF-8"?>')

    openNode(file, 'Root', 0)

    materials = getMaterials(objects, depsgraph, props)

    writeMaterials(file, materials, filepath)

    writeTopLevelObjects(file, objects, depsgraph, materials, scene, props)

    closeNode(file, 'Root')

    file.flush()
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

        exported = do_export(context, props, filepath)

        if exported:
            print('finished export in %s seconds' %((time.time() - start_time)))
            print(filepath)

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
