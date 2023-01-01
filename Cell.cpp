#include <iostream>
class Cell {
public:
    
    Cell(int x, int y) : x(x), y(y) {};
    // cell layer defaults to zero 
    // {amount of bombs around, button, flag}
    // cell with a bomb is represented as a -1
    int cell_layer[3] = {0, 1, 0}; 
    int x;
    int y;

    void leftClick() {
        // can't press cell that has a flag
        if (cell_layer[2]) return;
        else if (!cell_layer[1]) return;
        // can press it
        cell_layer[1] = 0; // remove button
        if (cell_layer[0] == -1) {
            std::cout << "You Lose!!!\n";
        }
    }
    
    void rightClick() {
        if (cell_layer[1]) cell_layer[2] = 1;
    }
    void addFlag() {
        // add flag only if button is available
        if (cell_layer[1]) {
            cell_layer[2] = 1;
        }
    }
private:
    
};
