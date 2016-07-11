// This JS file provides information to the local context of the plugins
// Please note that this is individual to each script file.

var ScriptInfo = {}
Object.defineProperty(ScriptInfo, "file", {
    value: "%{file_name}", // Replaced by the file name
    writable: false
});
Object.defineProperty(ScriptInfo, "path", {
    value: "%{path_name}", // Replaced by the path name
    writable: false
});
Object.defineProperty(ScriptInfo, "filePath", {
    value: "%{file_path_name}", // Replaced by the full file name (path + file name)
    writable: false
});



// ================= Below the code for main.js should be inserted ========================
