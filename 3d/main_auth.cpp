#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <gtkmm-3.0/gtkmm.h>
#include "myarea.h"
#include "loginWindow.h"
#include "SliceWindow.h"
#include "Polygons.h"


int main(int argc, char* argv[]) {
    try {

        std::cout <<"hello";
        Gtk::Main kit(argc, argv);
        SliceWindow window;
        window.set_default_size(1500,900);

        int id = 0;
        auto app = Gtk::Application::create("danilpiatov");
        // Connect to MySQL database
        soci::session sql("mysql", "db=soci_db user=soci_dev1 password=Secure123");
        LoginWindow loginWindow(sql, &id);

        app->run(loginWindow);

        if(id != 0) {
            Gtk::Main::run(window);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }


    return 0;
}
