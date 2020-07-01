
const binding = process.binding('atom_common_shell');
const Shell = binding.Shell;
exports.Shell = Shell;

const shell = binding.Shell;
exports.shell = shell;