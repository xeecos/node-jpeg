
const assert = require('assert');
const binding = require('./build/Release/jpeg');
assert.equal('world', binding.hello());
console.log('binding.hello() =', binding.hello());