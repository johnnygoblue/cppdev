#include <iostream>
#include <string>

#include <sqlite3.h> // Include the SQLite header file

// Define a struct to hold the log statistics
typedef struct LogStats {
    int date;
    std::string tt;
    int numberOfLogs;
    int maxActiveOrders;
    double meanActiveOrders;
    int medianActiveOrders;
    double stddevActiveOrders;
    double maxAmount;
    double meanAmount;
    double medianAmount;
    double minAmount;
    double stddevAmount;
} LogStats;

// function to execute an SQL statement and check for errors
void checkSQLiteError(int rc, sqlite3* db) {
    if (rc != SQLITE_OK) {
        std::cerr << "SQLite error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
}

// preapare LogStats record to be inserted into database table
void insertLogStats(sqlite3* db, std::string tb_name, const LogStats& stats) {
    std::string insertSQL = "INSERT OR REPLACE INTO " + tb_name + " (date, tt, "
        "numberOfLogs, maxActiveOrders, meanActiveOrders, medianActiveOrders, "
        "stddevActiveOrders, maxAmount, meanAmount, medianAmount, minAmount, "
        "stddevAmount) " "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, insertSQL.c_str(), -1, &stmt, nullptr);
    checkSQLiteError(rc, db);

    int cnt = 1;
    // Bind values to the parameters
    sqlite3_bind_int(stmt, cnt++, stats.date);
    sqlite3_bind_text(stmt, cnt++, stats.tt.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, cnt++, stats.numberOfLogs);
    sqlite3_bind_int(stmt, cnt++, stats.maxActiveOrders);
    sqlite3_bind_int(stmt, cnt++, stats.meanActiveOrders);
    sqlite3_bind_double(stmt, cnt++, stats.medianActiveOrders);
    sqlite3_bind_double(stmt, cnt++, stats.stddevActiveOrders);
    sqlite3_bind_double(stmt, cnt++, stats.maxAmount);
    sqlite3_bind_double(stmt, cnt++, stats.meanAmount);
    sqlite3_bind_double(stmt, cnt++, stats.medianAmount);
    sqlite3_bind_double(stmt, cnt++, stats.minAmount);
    sqlite3_bind_double(stmt, cnt++, stats.stddevAmount);

    // Execute the insert statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
    } else {
        std::cout << "Data inserted successfully." << std::endl;
    }

    // Finalize the statement to release resources
    sqlite3_finalize(stmt);
}

// opens database and create table if not already existent then inserts the 
// LogStats record into said table, closes databse after work is done
//
// Arguments:
//      <stats> log stats to be inserted
//      <tb_name> table name
//      <outputDB> path to database file
int write2db(LogStats stats, std::string tb_name, std::string outputDB) {
    sqlite3* db; // SQLite database object
    int rc;      // Result code

    // Open the SQLite database
    rc = sqlite3_open(outputDB.c_str(), &db); // Open a database file
    checkSQLiteError(rc, db);

    // Create the table with table name
    std::string createTableSQL = "CREATE TABLE IF NOT EXISTS " + tb_name + R"(
           (date INT PRIMARY KEY, 
            tt TEXT,
            numberOfLogs INT,
            maxActiveOrders INT,
            meanActiveOrders REAL,
            medianActiveOrders INT,
            stddevActiveOrders REAL,
            maxAmount REAL,
            meanAmount REAL,
            medianAmount REAL,
            minAmount REAL,
            stddevAmount REAL)
    )";
    rc = sqlite3_exec(db, createTableSQL.c_str(), nullptr, nullptr, nullptr);
    checkSQLiteError(rc, db);
    std::cout << "Table created successfully." << std::endl;

    // Insert the log statistics into the database
    insertLogStats(db, tb_name, stats);

    // Close the SQLite database
    sqlite3_close(db);

    return 0;
}

