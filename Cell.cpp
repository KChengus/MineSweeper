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

    bool leftClick() {
        // can't press cell that has a flag
        if (cell_layer[2]) return false;
        else if (!cell_layer[1]) return false;
        // can press it
        cell_layer[1] = 0; // remove button
        return cell_layer[0] == -1;
    }
    
    void rightClick() {
        if (cell_layer[1]) {
            cell_layer[2] = !cell_layer[2];
        }

    }
private:
    
};
