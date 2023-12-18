#include "client_interface.hpp"


WINDOW* ClientInterface::infoscr;
WINDOW* ClientInterface::chatscr;
WINDOW* ClientInterface::inptscr;

void ClientInterface::init()
{

    initscr();  

    // Initialize top, middle and bottom segment
    infoscr = newwin(        2, COLS - 1,         0, 0);
    chatscr = newwin(LINES - 5, COLS - 1,         2, 0); // Leave 2 lines for top and bottom segments
    inptscr = newwin(        2, COLS - 1, LINES - 2, 0);

    // Clear these screens to start with a blank canvas
    wclear(infoscr);
    wclear(chatscr);
    wclear(inptscr);

    // Enable scroll on chat screen
    scrollok(chatscr, true);

    // Print the UI components
    ClientInterface::printUI();

    std::signal(SIGWINCH, ClientInterface::handleResize);
}

void ClientInterface::destroy()
{
    // Delete all windows
    delwin(infoscr);
    delwin(chatscr);
    delwin(inptscr);

    refresh();
    endwin();
}

void ClientInterface::printMessage(std::string message)
{
    // Print message to screen
    wprintw(chatscr, (message + "\n").c_str());

    // Refresh screen
    wrefresh(chatscr);
}

void ClientInterface::printUI()
{
    std::string bottom_ui = "Say something: ";

    // Add UI at the bottom
    mvwaddstr(inptscr, 0, 0, bottom_ui.c_str());

    // Refresh screens
    wrefresh(infoscr);
    wrefresh(inptscr);
}

void ClientInterface::resetInput()
{

    // Clear user message
    wclear(inptscr);

    // Reprint the UI
    ClientInterface::printUI();

    // Refresh screen
    wrefresh(inptscr);
}

void ClientInterface::handleResize(int signum)
{
    // Resize windows according to new terminal size
    wresize(infoscr, 2, COLS - 1);
    wresize(chatscr, LINES - 5, COLS - 1);
    wresize(inptscr,    2, COLS - 1);

    // Refresh windows
    wrefresh(infoscr);
    wrefresh(chatscr);
    wrefresh(inptscr);
}