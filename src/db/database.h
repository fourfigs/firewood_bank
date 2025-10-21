#pragma once

#include <QSqlDatabase>
#include <QString>
#include <QVariantMap>

namespace firewood::db {

/**
 * @brief Opens the default database connection and runs migrations
 * @return QSqlDatabase instance (check isOpen() to verify success)
 */
QSqlDatabase openDefaultConnection();

/**
 * @brief Loads SQL script from a file and executes it
 * @param filePath Path to the SQL script file
 * @param db Database connection to use
 * @return true if successful, false otherwise
 */
bool loadSqlScript(const QString &filePath, QSqlDatabase &db);

/**
 * @brief Loads the sample data SQL script
 * @return true if successful, false otherwise
 */
bool loadSampleData();

/**
 * @brief Get sample data loading status
 * @return QVariantMap with status information
 */
QVariantMap getSampleDataStatus();

} // namespace firewood::db

