bl_info = {
    "name":         "XML Mesh Format",
    "author":       "Gunnar Zötl",
    "blender":      (2,6,2),
    "version":      (0,0,1),
    "location":     "File > Import-Export",
    "description":  "Export XML Mesh Format",
    "category":     "Import-Export"
}

import bpy
from bpy_extras.io_utils import ExportHelper

class ExportXMLMesh(bpy.types.Operator, ExportHelper):
    bl_idname       = "export_xml_mesh_format.mesh";
    bl_label        = "XML Mesh Exporter";
    bl_options      = {'PRESET'};

    filename_ext    = ".mesh";

    def execute(self, context):
        # Get a file handle
        file = open(self.filepath, 'w');
        # Write a line in the file
        file.write('This is a test!');
        # Close the file
        file.close();
        return {'Finished'};

def menu_func(self, context):
    self.layout.operator(ExportXMLMesh.bl_idname, text="XML Mesh Format(.mesh)");

def register():
    bpy.utils.register_module(__name__);
    bpy.types.INFO_MT_file_export.append(menu_func);

def unregister():
    bpy.utils.unregister_module(__name__);
    bpy.types.INFO_MT_file_export.remove(menu_func);

if __name__ == "__main__":
    register()
