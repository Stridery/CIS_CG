# Assignment2
## Step1-intersection function
### 1.Triangle intersection
In this function I used my own way to detect the intersection. 
First I check if the dirction of the ray is parallel with the triangle(perpendicular with the normal), if so, return false. 
Second, if not parallel, that means the ray is definitely hit somewhere on the plane where the triangle is, we call the point p. In this case, vector ap/bp/cp should be perpendicular to the normal, then Iget the t value. 
Then, I examine if p is inside the triangle. If it is, vector ap and ac are at the same side of ab, so cross(ap,ab)*cross(ac,ab)>0, and so on. After 3 checkings, if all true, then set the hitrecord and return true.

### 2.Sphere intersection
If p is on the sphere, length of cp is definitely rad. So I check if the quadratic equation has answers, or has only one answer(I assume the tangent occasion as miss). If it has 2 answers, I choose the smaller t, as it is in the front. 
After checking if it is valid, I set the hitrecord and return true.

### 3.Check what the ray hits
Traverse all surfaces, and choose the one with the smallest t to be hr.