#pragma once

#include <curses.h>
#include <string>
#include <cmath>
#include <csignal>

class ClientInterface
{
    private:
        static WINDOW* infoscr; // Top segment of the terminal, presents info about the current session
        static WINDOW* chatscr; // Middle segment of the terminal where incoming chat messages are shown


    public:
        static WINDOW* inptscr; // Bottom segment of the terminal where the user can type outgoing messages

        static void init();

        static void destroy();

        static void printMessage(std::string message);

        /**
         * Prints groupname and date on top of the screen, as well as user input at the bottom
         */
        static void printUI();

        /**
         * Resets the user input area below the screen
         * Deletes the old message and resets the cursor position
         */
        static void resetInput();

        /**
         * Handles a terminal resize signal 
         */
        static void handleResize(int signum);
};

