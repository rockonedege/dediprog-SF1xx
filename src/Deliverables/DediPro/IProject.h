#pragma once

struct IProject 
{
    virtual bool Init() = 0;
};

typedef IProject* HPROJECT;