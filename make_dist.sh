OS="`uname`"
case $OS in
  'Linux')
    OS='Linux'

    ;;
  'FreeBSD')
    OS='FreeBSD'
    ;;
  'WindowsNT')
    OS='Windows'
    ;;
  'Darwin') 
    OS='Mac'
    ;;
  'SunOS')
    OS='Solaris'
    ;;
  'AIX') ;;
  *) ;;
esac

export PATH=$PATH:~/Qt/5.12.0/clang_64/bin/

if [[ $OS == "Mac" ]]; then

  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./libraries/libopenmpt/lib/:./libraries/portaudio/lib/
  export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:./libraries/libopenmpt/lib/:./libraries/portaudio/lib/
  # Make the framework
# cd ~/Qt/5.12.0/Src
# ./configure -v  -static -mp -opensource -release -shared -framework  -sdk "macosx10.14" -confirm-license -prefix /www/qt/QtModPlayer/QTFramework
# make -j 10 && make install
  #@TODO: Automate compilation of libOpenMPT
  #@TODO: Automate build of portaudio
  rm -rf QtModPlayer.app
  qmake -config release 
  # make clean
  make -j 10
  
  mkdir -p QtModPlayer.app/Contents/Frameworks/
  cp -R libraries/* QtModPlayer.app/Contents/Frameworks/
  install_name_tool -change "/www/qt/libopenmpt-0.3.9_/build/lib/libopenmpt.0.dylib" "@rpath/libopenmpt/lib/libopenmpt.0.dylib" QtModPlayer.app/Contents/MacOS/QtModPlayer 
  install_name_tool -change "/usr/local/lib/libportaudio.2.dylib" "@rpath/portaudio/lib/libportaudio.dylib" QtModPlayer.app/Contents/MacOS/QtModPlayer 
  
  macdeployqt QtModPlayer.app \
  -codesign="Mac Developer: Jesus Garcia (N3R9A8VAKB)"
  # -libpath=./libraries/libopenmpt/lib/

# install_name_tool -id /www/qt/libopenmpt-0.3.9_/build/lib/libopenmpt.0.dylib QtModPlayer.app/Contents/Frameworks/libopenmpt/lib/libopenmpt.dylib 
  # make clean
fi
