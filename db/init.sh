rm -f ~/.QtModPlayer/*.db
echo "removed database from home directory"

sqlite3 modmusic.db < schema.sql
echo "Refreshed database schema"

rm -rf /www/qt/build-QtModPlayer-Desktop_Qt_5_11_0_clang_64bit2-Debug/QtModPlayer.app/
echo "deleted build file"

echo
