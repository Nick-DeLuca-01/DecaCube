////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  Assignment:     
//  Instructor:     David Burchill
//  Year / Term:    Fall 2024
//  File name:      Source.cpp
// 
//  Student name:   Nick DeLuca
//  Student email:  NDELUCA01@mynbcc.ca
// 
//     I certify that this work is my work only, any work copied from Stack Overflow, textbooks, 
//     or elsewhere is properly cited. 
// 
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  BUG 
//  list any and all bugs in your code 
//  1. I didnt get abc to work because of xyz, I tried ijk...
//

#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(1080, 640), "SFML works!"); //declares render window size

    sf::CircleShape shape(100.f, ); //creates a circle called shape, first is radius, second is point count
    shape.setFillColor(sf::Color::Green); //sets color

    while (window.isOpen()) //this is pretty straightforward
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}