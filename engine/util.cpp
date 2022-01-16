

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

    bool x_overlap = false;
    bool y_overlap = false;

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

bool collisionCrsq(float x1, float y1, float r1, float x2, float y2, float w2, float h2) {
    float h[4] = {
        x2 + r1 + w2 / 2,
        x2 + r1,
        x2 - r1,
        x2 - r1 - w2 / 2
    };

    float v[4] = {
        y2 + r1 + h2 / 2,
        y2 + r1,
        y2 - r1,
        y2 - r1 - h2 / 2
    };

    if(x1 > h[0] && x1 < h[3]) {
        //  within horizontal bounds
        if(y1 > v[0] && y1 < v[3]) {
            //  within vertical bounds
            if(y1 > v[1] && y1 < v[2]) {
                //  within horizontal range
                return true;
            }

            if(x1 > h[1] && x1 < h[2]) {
                //  within vertical range
                return true;
            }

            //  check circle corners
            if(((x1 - h[1]) * (x1 - h[1]) + (y1 - v[1]) * (y1 - v[1])) < r1 * r1) {
                return true;
            }

            if(((x1 - h[2]) * (x1 - h[2]) + (y1 - v[1]) * (y1 - v[1])) < r1 * r1) {
                return true;
            }

            if(((x1 - h[2]) * (x1 - h[2]) + (y1 - v[2]) * (y1 - v[2])) < r1 * r1) {
                return true;
            }

            if(((x1 - h[1]) * (x1 - h[1]) + (y1 - v[2]) * (y1 - v[2])) < r1 * r1) {
                return true;
            }

            
        }
    }


    return false;
}