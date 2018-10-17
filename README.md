# 割と酷い作りのassembler

# build command by emscripten
$ emcc -O1 lib.cpp logutil.cpp assembler.cpp -o assembler.html

# run
$ python -m SimpleHTTPServer 8000

and type this url on your browser.
http://localhost:8000/assembler.html

# info
https://qiita.com/kjunichi/items/47715431c1228bfdda90
