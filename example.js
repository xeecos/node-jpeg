
const binding = require('./build/Release/jpeg');
const buffer = Buffer.from("ABC");
const out = Buffer.alloc(3);
const width = 1920;
const height = 1080;
let size = binding.encode(buffer, buffer.length, out, out.length, width, height);
console.log('binding.encode() =', out[0],out[1],size);