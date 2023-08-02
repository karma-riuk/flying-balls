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
