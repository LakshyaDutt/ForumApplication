#include <iostream>
#include <string>
#include <regex>
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTTPRedirectHeader.h>
#include <cgicc/HTMLClasses.h>
#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include "check_strong_password.h"
#include "encrypt_password.h"
#include <openssl/sha.h>

using namespace std;
using namespace cgicc;

int main() {
    Cgicc cgi;
    std::cout << HTTPHTMLHeader() << std::endl;
    

    std::cout << "<html><body>" << std::endl;

    string flag;
    form_iterator name = cgi.getElement("name");
    form_iterator username = cgi.getElement("username");
    form_iterator email = cgi.getElement("email");
    form_iterator password = cgi.getElement("password");
    form_iterator repassword = cgi.getElement("repassword");
    string str_password = **password;
    string str_username = **username;
    string str_email = **email;
    string str_name = **name;
    string hash = sha512(str_password);
    if (str_username == "admin") {
        flag = "1";
    }
    else {
        flag = "0";
    }
    

    if (!checkStrongPassword(str_password)) {
        std::cout << "<p>Please enter a strong password and try again " << "</p>" << std::endl;
        cout << "</body></html>" << endl;
        return -1;
    }

    //string cookieValue = "2tg85967h34beuryia4t3g7968b43ug87zrvh9";
    //string cookieName = "Session-ID";
    //string cookiePath = "/";
    //string cookieExpires = "Wed, 21 Oct 2023 07:28:00 GMT";
    //HTTPCookie cookie(cookieName, cookieValue);
    //cookie.setPath(cookiePath);
    //cookie.setMaxAge(86400);
    //cout << cookie;
    string cookieValue = "";

    if (name != cgi.getElements().end() &&
        email != cgi.getElements().end() &&
        username != cgi.getElements().end() &&
        password != cgi.getElements().end()) {

        try {

            MYSQL conn;
            MYSQL_RES* rs;
            MYSQL_ROW row;
            mysql_init(&conn);

            if (!mysql_real_connect(&conn, "localhost", "root", "", "forum", 0, NULL, 0)) {
                fprintf(stderr, "Database Connection Failed!  Error: %s \n", mysql_error(&conn));
                mysql_close(&conn);
                return -1;
            }

            else {
                if (**username == "admin") {
                    string admin_query = "SELECT * FROM users_info where Username='admin'";
                    int admin_query_state = mysql_query(&conn, admin_query.c_str());
                    if (admin_query_state != 0) {
                        cerr << mysql_error(&conn) << endl;
                    }
                    else {
                        rs = mysql_store_result(&conn);
                        if (mysql_num_rows(rs) > 0) {
                            cout << "<p>Username unavailable, please select some other username!</p>" << endl;
                            cout << "</body></html>" << endl;
                            mysql_free_result(rs);
                            mysql_close(&conn);
                            return -1;
                        }
                    }
                }

                else {
                    string query = "SELECT * FROM users_info WHERE username='" + **username + "'";
                    int query_state = mysql_query(&conn, query.c_str());
                    if (query_state != 0) {
                        cerr << mysql_error(&conn) << endl;
                    }

                    else {
                        rs = mysql_store_result(&conn);

                        if (mysql_num_rows(rs) > 0) {
                            cout << "<p>Username already exists. Please choose a different username.</p>" << endl;
                            cout << "</body></html>" << endl;
                            mysql_free_result(rs);
                            mysql_close(&conn);
                            return -1;
                        }
                        else {
                            

                            string query = "INSERT INTO users_info (Name, Username, Email, Password, Cookies, PrivilegeFlag) VALUES ('" + **name + "','" + **username + "','" + **email + "','" + hash + "','" + cookieValue + "','" + flag + "');";
                            int query_state = mysql_query(&conn, query.c_str());
                            cout << HTTPRedirectHeader("http://127.0.0.1/cgi-bin/login.html") << endl;
                            if (query_state != 0) {
                                cerr << mysql_error(&conn) << endl;
                            }
                            
                        }
                    }
                }
            }
        }
        catch (exception& e) {
            cout << e.what() << endl;
        }
    }
    else {
        std::cout << "<p>No data received</p>" << std::endl;
    }

    cout << HTTPRedirectHeader("http://127.0.0.1/cgi-bin/login.html") << endl;
    std::cout << "</body></html>" << std::endl;
    return 0;
}