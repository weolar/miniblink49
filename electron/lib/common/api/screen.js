
const binding = process.binding('atom_common_screen');
const Screen = binding.Screen;
exports.Screen = new Screen();

const electronScreen = binding.Screen;
exports.screen = new electronScreen();