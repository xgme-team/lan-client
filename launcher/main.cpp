#include "application.h"
#include "mainwindow.h"
#include "model.h"


int main(int argc, char *argv[])
{
	Application a(argc, argv);
	return a.exec();
}
