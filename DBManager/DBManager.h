#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QtCore>
#include <QtSql>

#include <libopenmpt/libopenmpt.hpp>
#include <fstream>
#include <QJsonObject>

class DBManager : public QObject {
  Q_OBJECT

private:
  QSqlDatabase m_db;
  QString m_dbPath;
  QString m_dbFileName;
  QVector<QJsonObject *> m_filesToInsert;
  int m_playlistId;


public:
  explicit DBManager(QObject *parent = nullptr);
  void addToPlaylist(QString tableName, QJsonObject *modFile);
  void queueAddToPlaylist(int playlistId, QVector<QJsonObject *> filesToInsert);
  void bulkInsertToPlaylist(); // Executed by thread
  bool checkForDeployedDatabase();
  bool connect();
  bool disconnect();
  void deleteTable(QString tableName, int tableId);
  void purgeCurrentPlaylist();

  // Todo: Push into another class?
  int getNumRowsForPlaylist(QString tableName);
  QJsonObject *getRecordAt(int rowNumber, QString tableName);

  void deleteTable(QString tableName);

  QSqlDatabase db() const;
  void setDb(const QSqlDatabase &db);

  QString dbPath() const;
  void setDbPath(const QString &dbPath);

  QVector<QJsonObject *> filesToInsert() const;
  void setFilesToInsert(const QVector<QJsonObject *> &filesToInsert);

  int getSongCountFromPlaylist(QString tableName);

  /**** Playlist stuff ***/
  int playlistId() const;
  void setPlaylistId(int playlistId);
  int generateNewPlaylist(QString playlistName);

  QVector<QJsonObject *> getAllPlaylists(int newlyInsertedId = -1);


signals:
  void fileInserted();
  void insertPercentUpdate(int pctComplete);
  void insertComplete(int totalDone);

public slots:
};

#endif // DBMANAGER_H
