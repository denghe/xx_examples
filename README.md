# xx_examples
examples for xx game engine

#online demo

https://denghe.github.io/xx_examples/

#requirement ( windows )

1. TortoiseGit & git for win ( optional )
2. visual studio 2022 with c++ & game
3. cmake-gui latest
4. python3 latest
5. emsdk latest

https://github.com/emscripten-core/emsdk

1. git pull
2. open cmd window
3. .\emsdk.bat install latest
4. .\emsdk.bat activate latest --permanent
5. .\emsdk.bat list
6. find "mingw-?.?.?-64bit"
7. .\emsdk.bat install mingw-?.?.?-64bit
8. .\emsdk.bat activate mingw-?.?.?-64bit --permanent
9. close cmd window

#git usage

1. locate directory
2. mouse right menu choose: GitSync
3. click "Pull"
4. click "Submodule Update"

#tips

upgrade all submode to latest version:

git submodule update --remote
