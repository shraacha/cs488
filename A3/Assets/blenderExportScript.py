import bpy
import math

def get_mesh_name(obj) -> str:
    data_name = obj.data.name

    if 'Cube' in data_name:
        return 'blenderCube'

    if 'Pyramid' in data_name:
        return 'pyramid'

    if 'Cone' in data_name:
        return 'cone'

    if 'Cylinder' in data_name:
        return 'cylinder'

    if 'Sphere' in data_name:
        return 'sphere'

    if 'TriangularPrism' in data_name:
        return 'triangularPrism'


def write_some_data(context, filepath):
    print("running write_some_data...")
    f = open(filepath, 'w', encoding='utf-8')

    for obj in bpy.context.selected_objects:
        name = obj.name

        scale = obj.scale.xzy

        rotation_x = obj.rotation_euler.x
        rotation_y = obj.rotation_euler.z
        rotation_z = -obj.rotation_euler.y

        translation = obj.location.xzy
        translation.z = -translation.z

        f.write(f'{name} = gr.mesh(\'{get_mesh_name(obj)}\', \'{name}\')\n')
        f.write(f'<ROOT>:add_child({name})\n')
        f.write(f'{name}:scale({scale.x}, {scale.y}, {scale.z})\n')
        f.write(f'{name}:rotate(\'x\', {math.degrees(rotation_x)})\n')
        f.write(f'{name}:rotate(\'z\', {math.degrees(rotation_z)})\n')
        f.write(f'{name}:rotate(\'y\', {math.degrees(rotation_y)})\n')
        f.write(f'{name}:translate({translation.x}, {translation.y}, {translation.z})\n')
        f.write(f'{name}:set_material(<MATERIAL>)\n')
        f.write('\n')

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportSomeData(Operator, ExportHelper):
    """Export to be added to a lua file for CS488 A3"""
    bl_idname = "export_test.some_data"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export Some Data"

    # ExportHelper mix-in class uses this.
    filename_ext = ".txt"

    filter_glob: StringProperty(
        default="*.txt",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    def execute(self, context):
        return write_some_data(context, self.filepath)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
#    self.layout.operator(ExportSomeData.bl_idname, text="CS488 Export")
    return


# Register and add to the "file selector" menu (required to use F3 search "Text Export Operator" for quick access).
def register():
    bpy.utils.register_class(ExportSomeData)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportSomeData)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()

    # test call
    bpy.ops.export_test.some_data('INVOKE_DEFAULT')
