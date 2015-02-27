RayTracer Notes
----------------

Transparency/Refractions - Objects that are transparent will modulate the color behind them at a refractive index provided by the paramater
							in the configuration. Transparent objects can still reflect but are not blinn-phong lit, and cast shadows.
							
Fresnel Refraction - Code for Fresnel refractions is also in there and I think its working as well, but it has been difficult to test.
					For Sure I am getting more reflections toward areas where the the normal and the angle are perp,
					and i am getting more transmission where the normals are parallel, so I think it is working correctly.
					
					Note: to test out just transparency with reflection you can comment out the fresnel color contribution and uncomment the line right below it
					in TraceRay().
