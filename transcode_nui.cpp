#include <ncurses.h>
void displayStaticContent() {
	mvwprintw(stdscr, 7, 0, "|   Output URL  ");
	mvwprintw(stdscr, 3, 0, "|                                                                             |");
	mvwprintw(stdscr, 8, 0, "|                                                                             |");
	mvwprintw(stdscr, 21, 0, "|                                                                             |");
	mvwprintw(stdscr, 23, 0, "|                                                                             |");
	mvwprintw(stdscr, 27, 0, "|          ('x' => exit                            )                          |");
	mvwprintw(stdscr, 26, 0, "|          ('c' => connect input and output streams)                          |");
	mvwprintw(stdscr, 16, 0, "| Stream Type ");
	mvwprintw(stdscr, 16, 39, "| Stream Type ");
	mvwprintw(stdscr, 10, 0, "|                                      |                                      |");
	mvwprintw(stdscr, 11, 0, "|                                      |                                      |");
	mvwprintw(stdscr, 18, 0, "|                                      |                                      |");
	mvwprintw(stdscr, 19, 0, "|                                      |                                      |");
	mvwprintw(stdscr, 5, 0, "|   Stream Name ");
	mvwprintw(stdscr, 1, 0, "|   StreamTranscoder                   http");
	mvwprintw(stdscr, 25, 0, "| Commands                                                                    |");
	mvwprintw(stdscr, 13, 0, "| Total Read ");
	mvwprintw(stdscr, 13, 39, "| Total Read ");
	mvwprintw(stdscr, 6, 0, "|   MetaData    ");
	mvwprintw(stdscr, 15, 0, "| Server Type ");
	mvwprintw(stdscr, 15, 39, "| Server Type ");
	mvwprintw(stdscr, 0, 0, "|-----------------------------------------------------------------------------|");
	mvwprintw(stdscr, 2, 0, "|-----------------------------------------------------------------------------|");
	mvwprintw(stdscr, 9, 0, "|-----------------------------------------------------------------------------|");
	mvwprintw(stdscr, 20, 0, "|-----------------------------------------------------------------------------|");
	mvwprintw(stdscr, 24, 0, "|-----------------------------------------------------------------------------|");
	mvwprintw(stdscr, 28, 0, "|-----------------------------------------------------------------------------|");
	mvwprintw(stdscr, 14, 0, "| BPS Read ");
	mvwprintw(stdscr, 14, 39, "| BPS Read ");
	mvwprintw(stdscr, 12, 0, "| Status ");
	mvwprintw(stdscr, 12, 39, "| Status ");
	mvwprintw(stdscr, 22, 0, "| Status ");
	mvwprintw(stdscr, 4, 0, "|   Input URL   ");
	mvwprintw(stdscr, 1, 44, "//transcoder.sourceforge.net      |");
	mvwprintw(stdscr, 17, 0, "| Bitrate ");
	mvwprintw(stdscr, 17, 39, "| Bitrate ");
	mvwprintw(stdscr, 4, 78, "|");
	mvwprintw(stdscr, 5, 78, "|");
	mvwprintw(stdscr, 6, 78, "|");
	mvwprintw(stdscr, 7, 78, "|");
	mvwprintw(stdscr, 12, 78, "|");
	mvwprintw(stdscr, 13, 78, "|");
	mvwprintw(stdscr, 14, 78, "|");
	mvwprintw(stdscr, 15, 78, "|");
	mvwprintw(stdscr, 16, 78, "|");
	mvwprintw(stdscr, 17, 78, "|");
	mvwprintw(stdscr, 22, 78, "|");
}
void setoutputURL(char *pData) {
	char	field[57];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 7, 18, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 56);
	attron(A_BOLD);
	mvwprintw(stdscr, 7, 18, field);
	attroff(A_BOLD);
}
void setoutputTotalRead(char *pData) {
	char	field[23];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 13, 54, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 22);
	attron(A_BOLD);
	mvwprintw(stdscr, 13, 54, field);
	attroff(A_BOLD);
}
void setoutputBitrate(char *pData) {
	char	field[26];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 17, 51, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 25);
	attron(A_BOLD);
	mvwprintw(stdscr, 17, 51, field);
	attroff(A_BOLD);
}
void setoutputStatus(char *pData) {
	char	field[27];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 12, 50, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 26);
	attron(A_BOLD);
	mvwprintw(stdscr, 12, 50, field);
	attroff(A_BOLD);
}
void setstreamName(char *pData) {
	char	field[57];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 5, 18, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 56);
	attron(A_BOLD);
	mvwprintw(stdscr, 5, 18, field);
	attroff(A_BOLD);
}
void setinputTotalRead(char *pData) {
	char	field[23];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 13, 15, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 22);
	attron(A_BOLD);
	mvwprintw(stdscr, 13, 15, field);
	attroff(A_BOLD);
}
void setmainStatus(char *pData) {
	char	field[66];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 22, 11, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 65);
	attron(A_BOLD);
	mvwprintw(stdscr, 22, 11, field);
	attroff(A_BOLD);
}
void setinputBitrate(char *pData) {
	char	field[26];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 17, 12, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 25);
	attron(A_BOLD);
	mvwprintw(stdscr, 17, 12, field);
	attroff(A_BOLD);
}
void setoutputBPSRead(char *pData) {
	char	field[25];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 14, 52, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 24);
	attron(A_BOLD);
	mvwprintw(stdscr, 14, 52, field);
	attroff(A_BOLD);
}
void setstreamURL(char *pData) {
	char	field[57];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 4, 18, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 56);
	attron(A_BOLD);
	mvwprintw(stdscr, 4, 18, field);
	attroff(A_BOLD);
}
void setinputStatus(char *pData) {
	char	field[27];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 12, 11, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 26);
	attron(A_BOLD);
	mvwprintw(stdscr, 12, 11, field);
	attroff(A_BOLD);
}
void setoutputStreamType(char *pData) {
	char	field[22];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 16, 55, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 21);
	attron(A_BOLD);
	mvwprintw(stdscr, 16, 55, field);
	attroff(A_BOLD);
}
void setoutputServerType(char *pData) {
	char	field[22];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 15, 55, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 21);
	attron(A_BOLD);
	mvwprintw(stdscr, 15, 55, field);
	attroff(A_BOLD);
}
void setinputStreamType(char *pData) {
	char	field[22];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 16, 16, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 21);
	attron(A_BOLD);
	mvwprintw(stdscr, 16, 16, field);
	attroff(A_BOLD);
}
void setinputBPSRead(char *pData) {
	char	field[25];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 14, 13, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 24);
	attron(A_BOLD);
	mvwprintw(stdscr, 14, 13, field);
	attroff(A_BOLD);
}
void setinputServerType(char *pData) {
	char	field[22];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 15, 16, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 21);
	attron(A_BOLD);
	mvwprintw(stdscr, 15, 16, field);
	attroff(A_BOLD);
}
void setmetaData(char *pData) {
	char	field[57];

	memset(field, '\000', sizeof(field));
	memset(field, ' ', sizeof(field)-1);
	mvwprintw(stdscr, 6, 18, field);
	memset(field, '\000', sizeof(field));
	strncpy(field, pData, 56);
	attron(A_BOLD);
	mvwprintw(stdscr, 6, 18, field);
	attroff(A_BOLD);
}
