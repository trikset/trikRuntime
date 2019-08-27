script.removeFile("test.txt");
var x = 239;
script.writeToFile("test.txt", x);
var lines = script.readAll("test.txt");
var y = lines[0];

assert(y == 239);
