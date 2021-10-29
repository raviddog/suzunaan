

bool collisionSqsq(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    //  assumes centered x/y
    /*
     *          y1 - h1/2
     *  x1 - w1/2       x1 + w1/2
     *          y1 + h1/2
     *
     * 
     * 
     * 
     * 
     * 
     * */

    bool x_overlap = true;
    bool y_overlap = true;

    if(x1 + w1 / 2 < x2 - w2 / 2) {
        return false;
    } else if(x1 - w1 / 2 > x2 + w2 / 2) {
        return false;
    } else {
        x_overlap = true;
    }

    if(y1 - h1 / 2 > y2 + h2 / 2) {
        return false;
    } else if(y1 + h1 / 2 < y2 - h2 / 2) {
        return false;
    } else {
        y_overlap = true;
    }

    return true;

    
}