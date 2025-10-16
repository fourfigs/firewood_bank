#pragma once

#include <QSqlDatabase>

namespace firewood::db {

/**
 * @brief Opens the default database connection and runs migrations
 * @return QSqlDatabase instance (check isOpen() to verify success)
 */
QSqlDatabase openDefaultConnection();

} // namespace firewood::db

