
#include "cursesINDI.hpp"


   


int main()
{
//    FILE *f = fopen("/dev/tty", "r+");
//    SCREEN *screen = newterm(NULL, f, f);
//    set_term(screen);
  
   initscr();
   cbreak();
   raw();    /* Line buffering disabled*/
   keypad(stdscr, TRUE); /* We get F1, 2 etc...*/
   
   noecho(); /* Don't echo() while we do getch */
   
   
   cursesINDI ci("me", "1.7", "1.7");

   ci.m_tabHeight = 20;
   ci.m_tabX = 1;
   ci.m_tabWidth = 78;
   
   ci.processIndiRequests(true);
   ci.activate();
 
   
   
   pcf::IndiProperty ipSend;
   ci.sendGetProperties( ipSend );
      
   WINDOW * topWin = newwin(2, COLS, 0, 0);
   wprintw(topWin, "Press q to quit");
   keypad(topWin, TRUE);
   wrefresh(topWin);
   
   WINDOW * boxWin = newwin(ci.m_tabHeight+2, ci.m_tabWidth+2, 4,0);
   box(boxWin, 0, 0);
   wrefresh(boxWin);
   int ch;
   

   curs_set(0);
   
   //Now main event loop
   while((ch = wgetch(topWin)) != 'q')
   { 
      int nextX = ci.m_currX;
      int nextY = ci.m_currY;
      
      
      switch(ch)
      { 
         case KEY_LEFT:
            --nextX;
            break;
         case KEY_RIGHT:
            ++nextX;
            break;
         case KEY_UP:
            --nextY;
            break;
         case KEY_DOWN:
            ++nextY;
            break;
         default:
            ci.keyPressed(ch);
            continue;
            break;
      }

      int maxX = ci.m_cx.size()-1;
      if(nextX < 1) nextX = 1;
      if(nextX >= maxX) nextX = maxX;
      
      int maxY = ci.rows.size();
      if(nextY < 0) nextY = 0;
      if(nextY >= maxY) nextY = maxY - 1;

      if(nextY-ci.m_currFirstRow > ci.m_tabHeight-1)
      {
         ci.updateRowY(nextY - ci.m_tabHeight + 1);
      }
      else if( nextY < ci.m_currFirstRow)
      {
         ci.updateRowY(nextY);
      }
      
      ci.moveCurrent(nextY, nextX);
      
      wrefresh(topWin);
      
      curs_set(0);
   }

   
   ci.shutDown();
   
   endwin();   /* End curses mode */
   //fclose(f);
   
   
   
}