#include <iostream>
#include "gridwindow.h"

using namespace std;

// Constructor for window. It constructs the three portions of the GUI and lays them out vertically.
GridWindow::GridWindow(QWidget *parent,int rows,int cols)
: QWidget(parent)
{
    QHBoxLayout *header = setupHeader();            // Setup the title at the top.
    QGridLayout *grid = setupGrid(rows,cols);       // Setup the grid of colored cells in the middle.
    QHBoxLayout *buttonRow = setupButtonRow();      // Setup the row of buttons across the bottom.
    QVBoxLayout *layout = new QVBoxLayout();        // Put it all onto one box.
    layout->addLayout(header);
    layout->addLayout(grid);
    layout->addLayout(buttonRow);
    setLayout(layout);
}

// Destructor.
GridWindow::~GridWindow()
{
    delete title;
}

// Builds header section of the GUI.  
QHBoxLayout* GridWindow::setupHeader()
{
    QHBoxLayout *header = new QHBoxLayout();            // Creates horizontal box.
    header->setAlignment(Qt::AlignHCenter);

    this->title = new QLabel("CONWAY'S GAME OF LIFE",this);             // Title: "Conway's Game of Life."
    this->title->setAlignment(Qt::AlignHCenter);
    this->title->setFont(QFont("Arial", 32, QFont::Bold));

    header->addWidget(this->title);     // Adds widget to layout.

    return header;                      // Returns header to grid window.
}

// Builds the grid of cells.  This method populates the grid's 2D array of GridCells with MxN cells.
QGridLayout* GridWindow::setupGrid(int rows,int cols)
{
    QGridLayout *grid = new QGridLayout();      // Creates grid layout.

    grid->setHorizontalSpacing(0);              // No empty spaces. Cells should be contiguous.
    grid->setVerticalSpacing(0);
    grid->setSpacing(0);
    grid->setAlignment(Qt::AlignHCenter);

    for(int i=0; i < rows; i++)                     //Each row is a vector of grid cells.
    {
        std::vector<GridCell*> row;                 // Creates new vector for current row.
        cells.push_back(row);
        for(int j=0; j < cols; j++)
        {
            GridCell *cell = new GridCell();        // Creates and adds new cell to row.
            cells.at(i).push_back(cell);

            grid->addWidget(cell,i,j);              // Adds to cell to grid layout. Column expands vertically.
            grid->setColumnStretch(j,1);
        }
        grid->setRowStretch(i,1);                   // Sets row expansion horizontally.
    }
    return grid;                                    // Returns grid.
}

// Builds footer section of the GUI.  
QHBoxLayout* GridWindow::setupButtonRow()
{
    QHBoxLayout *buttonRow = new QHBoxLayout();     // Creates horizontal box for buttons.
    buttonRow->setAlignment(Qt::AlignHCenter);

    // Clear Button - Clears cell; sets them all to DEAD/white.
    QPushButton *clearButton = new QPushButton("CLEAR");
    clearButton->setFixedSize(100,25); 
    connect(clearButton, SIGNAL(clicked()), this, SLOT(handleClear()));     
    buttonRow->addWidget(clearButton);

    // Start Button - Starts game when user clicks. Or, resumes game after being paused.
    QPushButton *startButton = new QPushButton("START/RESUME");
    startButton->setFixedSize(100,25);              
    connect(startButton, SIGNAL(clicked()), this, SLOT(handleStart()));     
    buttonRow->addWidget(startButton);

    // Pause Button - Pauses simulation of game.
    QPushButton *pauseButton = new QPushButton("PAUSE");
    pauseButton->setFixedSize(100,25);              
    connect(pauseButton, SIGNAL(clicked()), this, SLOT(handlePause()));     
    buttonRow->addWidget(pauseButton);  

    // Quit Button - Exits program.
    QPushButton *quitButton = new QPushButton("EXIT");
    quitButton->setFixedSize(100,25); 
    connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));     
    buttonRow->addWidget(quitButton);

    return buttonRow;               // Returns bottom of layout.
}

/*
    SLOT method for handling clicks on the "clear" button. 
    Receives "clicked" signals on the "Clear" button and sets all cells to DEAD.
*/
void GridWindow::handleClear()
{
    for(unsigned int row=0; row < cells.size(); row++)          // Loops through current rows' cells.
    {
    for(unsigned int col=0; col < cells[row].size(); col++)
        {
            GridCell *cell = cells[row][col];                   // Grab the current cell & set its value to dead.
            cell->setType(DEAD);
        }
    }
}

/*
    SLOT method for handling clicks on the "start" button. 
    Receives "clicked" signals on the "start" button and begins game simulation.
*/
void GridWindow::handleStart()
{
    this->timer = new QTimer(this);                                     // Creates new timer.
    connect(this->timer, SIGNAL(timeout()), this, SLOT(timerFired()));  // Connect "timerFired" method class to the "timeout" signal fired by the timer.
    this->timer->start(500);                                            // Timer to fire every 500 milliseconds.
}

/*
    SLOT method for handling clicks on the "pause" button. 
    Receives "clicked" signals on the "pause" button and stops the game simulation.
*/
void GridWindow::handlePause()
{
        this->timer->stop();        // Stops the timer.
        delete this->timer;         // Deletes timer.
}

// Accessor method - Gets the 2D vector of grid cells.
std::vector<std::vector<GridCell*> >& GridWindow::getCells()
{
    return this->cells;
}

void GridWindow::timerFired()
{
    //Store flags that represent whether the new grid cells will be live or not
	vector< vector<bool> > is_live(cells.size());
	for(int m=0; m < static_cast<int>(cells.size()); m++)
	{
    	is_live.at(m).resize(cells.at(m).size());
    	for(int n=0; n < static_cast<int>(cells.at(m).size()); n++)
    	{
        	//count neighbors
        	unsigned int neighbors = 0;
        	for(int i=-1; i<=1; i++)
        	{
            	for(int j=-1; j<=1; j++)
            	{
            		if (((m + i) >= 0 || (n + j) >= 0) || 
            			(((m + i) < static_cast<int>(cells.size())) 
            				|| (n + j) < static_cast<int>(cells.at(m).size())))
            		{
            			neighbors += static_cast<int>(
                    	cells[m+i][n+j]->getType() == LIVE);
            		}
       		    }
        	}
        	//we counted the current cell when counting the neighbors so
        	//subtract it back off if needed.
        	neighbors -= static_cast<int>(cells[m][n]->getType() == LIVE);

	        //Set the type to the original value
	        is_live[m][n] = cells[m][n]->getType() == LIVE;

	        //change it based on the neighbor count.
	        //Some of your logic around here seemed repetitive so I 
	        //did it differently.  You may want to change it back 
	        //if you had a specific purpose for the way you did it
	        is_live[m][n] = (is_live[m][n] && neighbors <= 3) || 
                        (!is_live[m][n] && neighbors == 3);
		}
	}

	//Set the cell types based on the is_live flags.
	for(int m=0; m < static_cast<int>(cells.size()); m++)
	{
    	for(int n=0; n < static_cast<int>(cells.at(m).size()); n++)
    	    cells[m][n]->setType(is_live[m][n] ? LIVE : DEAD);
	}
}