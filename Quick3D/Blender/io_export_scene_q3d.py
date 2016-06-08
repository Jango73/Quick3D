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
    "version": (1, 1, 0),
    "blender": (2, 71, 0),
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

def writeMaterial(file, mat, filepath):
    source_dir = os.path.dirname(bpy.data.filepath)

    openNode(file, 'Material', 0, Name = mat.name)

    diffuse_map = ''

    if len(mat.texture_slots) > 0:
        if mat.texture_slots[0] is not None:
            if mat.texture_slots[0].texture.type == 'IMAGE':
                head, tail = os.path.split(mat.texture_slots[0].texture.image.filepath)
                diffuse_map = 'Textures/' + tail
                # image = mat.texture_slots[0].texture.image
                # diffuse_map = bpy_extras.io_utils.path_reference(image.filepath, source_dir, filepath)

    openNode(file, 'Ambient', 1,
        r = '{0:.4f}'.format(mat.ambient),
        g = '{0:.4f}'.format(mat.ambient),
        b = '{0:.4f}'.format(mat.ambient)
        )

    openNode(file, 'Diffuse', 1,
        r = '{0:.4f}'.format(mat.diffuse_color[0]),
        g = '{0:.4f}'.format(mat.diffuse_color[1]),
        b = '{0:.4f}'.format(mat.diffuse_color[2]),
        Map = diffuse_map
        )

    openNode(file, 'Specular', 1,
        r = '{0:.4f}'.format(mat.specular_color[0]),
        g = '{0:.4f}'.format(mat.specular_color[1]),
        b = '{0:.4f}'.format(mat.specular_color[2]),
        Intensity = mat.specular_intensity,
        Hardness = mat.specular_hardness
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

def writeMesh(file, object, materials, scene, props):
    mat_x90 = mathutils.Matrix.Rotation(-math.pi/2, 4, 'X')

    try:
        mesh = object.to_mesh(scene, props.apply_modifiers, 'PREVIEW', calc_tessface=False)
    except RuntimeError:
        mesh = None

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
        writeSingleObject(file, child, materials, scene, props)

    closeNode(file, 'Component')

#--------------------------------------------------------------------------------------------------

def writeBone(file, bone, materials, scene, props):
    openNode(file, 'Bone', 0, Name=bone.name)

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

    closeNode(file, 'Bone')

#--------------------------------------------------------------------------------------------------

def writeArmature(file, object, materials, scene, props):
    openNode(file, 'Component', 0, Name=object.name, Class="CArmature")

    armature = object.data

    for bone in armature.bones:
        if bone.parent is None:
            writeBone(file, bone, materials, scene, props)

    # Write child objects
    for child in object.children:
        writeSingleObject(file, child, materials, scene, props)

    closeNode(file, 'Component')

#--------------------------------------------------------------------------------------------------

def writeSingleObject(file, object, materials, scene, props):
    if object.type == 'ARMATURE':
        writeArmature(file, object, materials, scene, props)
    else:
        writeMesh(file, object, materials, scene, props)

#--------------------------------------------------------------------------------------------------

def writeTopLevelObjects(file, objects, materials, scene, props):
    for object in objects:
        if object.parent is None:
            writeSingleObject(file, object, materials, scene, props)

#--------------------------------------------------------------------------------------------------

def getMaterials(objects, scene, props):
    materials = []

    for object in objects:

        try:
            mesh = object.to_mesh(scene, props.apply_modifiers, 'PREVIEW', calc_tessface=False)
        except RuntimeError:
            mesh = None

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
    start = props.range_start
    end = props.range_end
    sampling = float(props.sampling)

    objects = scene.objects

    file = open(filepath, "wt")
    file.indent = 0

    # Write XML Header
    writeLine(file, '<?xml version="1.0" encoding="UTF-8"?>')

    openNode(file, 'Root', 0)

    materials = getMaterials(objects, scene, props)

    writeMaterials(file, materials, filepath)

    writeTopLevelObjects(file, objects, materials, scene, props)

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

    rot_x90 = BoolProperty(name="Convert to Y-up",
            description="Rotate 90 degrees around X to convert to y-up",
            default=True,
            )
    world_space = BoolProperty(name="Export into Worldspace",
            description="Transform the Vertexcoordinates into Worldspace",
            default=False,
            )
    apply_modifiers = BoolProperty(name="Apply Modifiers",
            description="Applies the Modifiers",
            default=True,
            )
    range_start = IntProperty(name='Start Frame',
            description='First frame to use for Export',
            default=1,
            )
    range_end = IntProperty(name='End Frame',
            description='Last frame to use for Export',
            default=250,
            )
    sampling = EnumProperty(name='Sampling',
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
        return context.active_object.type in {'MESH', 'CURVE', 'SURFACE', 'FONT', 'ARMATURE'}

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

        if True:
            # File selector
            wm.fileselect_add(self) # will run self.execute()
            return {'RUNNING_MODAL'}
        elif True:
            # search the enum
            wm.invoke_search_popup(self)
            return {'RUNNING_MODAL'}
        elif False:
            # Redo popup
            return wm.invoke_props_popup(self, event)
        elif False:
            return self.execute(context)

### REGISTER ###

def menu_func(self, context):
    self.layout.operator(Export_q3d.bl_idname, text="Quick3D (.q3d)")

def register():
    bpy.utils.register_module(__name__)

    bpy.types.INFO_MT_file_export.append(menu_func)
    #bpy.types.VIEW3D_PT_tools_objectmode.prepend(menu_func)

def unregister():
    bpy.utils.unregister_module(__name__)

    bpy.types.INFO_MT_file_export.remove(menu_func)
    #bpy.types.VIEW3D_PT_tools_objectmode.remove(menu_func)

if __name__ == "__main__":
    register()
