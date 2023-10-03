# CS488 Fall 2023 Assignment 1 - README

---

## Introduction

This README provides instructions for compiling and running the CS488/688 assignment 2 and additional information about its features.

---

## Compilation
To compile and run the program, follow these steps:

1. **Unzip the A1.zip File:** 

2. **Execute default premake4/make combination:** 

    ```bash
    $ cd A2
    $ premake4 gmake
    $ make
    $ ./A2
    ```

I've accomplished this assignment on a Mac M1 and verified the code's functionality on the lab machine gl02.

---

# Manual

This manual provides additional details and features beyond the assignment specification.

### Projection

1. **Apploy Projection Matrix:**
To elevate the visual appeal of our project, I've introduced a simple Phong lighting model. This model comprises three essential components:

    - **Ambient Lighting**: Creates a subtle overall illumination.
    - **Diffuse Lighting**: Simulates the effect of light scattering on surfaces.
    - **Specular Lighting**: Mimics the shiny highlights on objects.

When you run the program, you'll notice a small white cube positioned at (0, 10, 0) – that's our light source, contributing to the dynamic visual experience. You can find all the lighting details in the vertex and fragment shader files.


