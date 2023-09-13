# CS488 Fall 2023 Assignment 0 - README

---

## Compilation
To compile and run the program, follow these steps:

1. **Unzip the A0.zip File:** 

2. **Execute default premake4/make combination:** 

    ```bash
    $ cd /path/to/A0
    $ premake4 gmake
    $ make
    $ ./A0
    ```

I successfully completed this assignment on a Mac M1, and tested the code in the lab machine ....

---

## Manual
This manual provides additional details and features beyond the assignment specification.

### Additional Features
1. **Shape Size Slider:**
    - I have implemented a slider that allows you to adjust the size of the triangle dynamically.
    - The valid range for the size is between 0.0 and 10.0.
    - The default size of the shape is set to 1.

2. **Position Information:**
    - I've added a text message display that shows the current X and Y position of the triangle in the graphical interface.

3. **Keyboard Shortcuts:**
    - Introduced new keyboard shortcuts for enhanced interaction:
        - "Up or W": Moves the triangle upward by 0.1 units.
        - "Down or S": Moves the triangle downward by 0.1 units.
        - "Left or A": Moves the triangle to the left by 0.1 units.
        - "Right or D": Moves the triangle to the right by 0.1 units.
        - "Space": Rotates the triangle by ( PI  / 10 ) radians.

