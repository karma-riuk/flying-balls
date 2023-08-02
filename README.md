# From Flying Balls to Colliding Polygons

## Abstract

> (text taken from the abstract of my report that you can find
> [here](https://github.com/karma-riuk/bachelor-project-report/blob/main/bachelorproject.pdf)

Physics engines are a fun and interesting way to learn about the laws of
physics, as well as computer science. They provide a real-time simulation of
common physical phenomena, and therefore illustrate theoretical concepts such as
the equations that dictate the motion of objects.

The goal of this project was to extend an existing physics engine built for
demonstration purposes. This engine was initially designed and developed to
simulate circular objects ("balls") in 2D.

With this project, we intended to extend this engine to also simulate arbitrary
polygons, again in a physically accurate way. The main technical challenges of
the project is therefore the correct simulation of the dynamics of rigid,
polygonal objects. In particular, we developed a model of polygonal rigid
objects:

- we implemented a simulation of their inertial motion, possibly in the
  presence of a constant force field such as gravity;
- we detect collisions between objects;
- we compute and then simulate the dynamic effects of collisions.

The simulations are animated and displayed in real-time. It is also therefore
crucial that the simulation code be efficient to obtain smooth animations.

## Before vs After

A little of context for the follow videos:

- in the first video, the special ball with an arrow inside is a "spaceship",
  that can be controlled by the user;
- the yellow bar that appears after midway through both videos represent the
  restitution coefficient of the collision resolution (the lower it is, the
  greater the dampening on impact between objects);
- the white line with a ball at the end that appears after the restitution
  coefficient bar represents the gravity vector that gets applied to the speed
  of each object at each frame, the ball is the direction the vector is pointing
  it.

### Before

[before video](before.mp4)

### After

[after video](after.mp4)

<!-- ## Controls -->
<!---->
<!-- ## Installation -->

## Contact

If you have any question concerning this work, feel free to contact me at [arno.fauconnet@gmail.com](mailto:arno.fauconnet@gmail.com)
