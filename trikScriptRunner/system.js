var KeysEnum = {
    Left : 105,
    Up : 103,
    Down : 108,
    Enter : 28,
    Right : 106,
    Power : 116,
    Menu : 139
};

// String.format function implementation.
// Taken from http://stackoverflow.com/questions/610406/javascript-equivalent-to-printf-string-format
if (!String.prototype.format) {
  String.prototype.format = function() {
    var args = arguments;
    return this.replace(/{(\d+)}/g, function(match, number) {
      return typeof args[number] != 'undefined'
        ? args[number]
        : match
      ;
    });
  };
}
