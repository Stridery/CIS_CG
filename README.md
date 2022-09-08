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

## Step2-shading
When the ray from the camera hits something, using the original Phong BRDF model, I can calculate the color of the hitten point. The color composes of diffuse reflection and specular reflection. In this assignnment I ignore the ambient 
part as there is no environmental effect. I send out a ray towards each light source, if it is not blocked, I calculate each vectors (as shown in the graph below), and get the 
color = max(cos(I,n),0)*lightcolor*fillcolor*kd+max(cos(v,r),0)*lightcolor*fillcolor*ks.

## Step3-checking shadows
When I send out a ray towards each light source, traverse all the objects to see if the ray hit anything between the hitten point and the light source. If it dose, skip the shading part, so that it appearces to be in a shadow.

##Step4-Calculating reflection
In this part I use recursive function. When I finished calculation of the first point, let it be the new eye, and send out a ray towards the reflection direction. If it hits something, 
I do all the process again, and add up to the color of the first point. By this way, I can do reflection calculating no matter how many times I want, just let trace function call shade function, and shde function call trace function again after shading 
is done. However, to make sure the process ends, I set a variable c, c++ when a cycle is done. When c equals maxraydepth, return black color and end. And when c is not up to maxraydepth, but a ray hit nothing in trace function, return black 
color and end as well. Specially when the first ray hit nothing, return background color.

## Bounding box accelerating
To implement this part, I finally chose to design a well-proportioned bounding box.
