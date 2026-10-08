#include <iostream>
#include <vector>

#include "CGL/vector2D.h"

#include "mass.h"
#include "rope.h"
#include "spring.h"

namespace CGL {

    Rope::Rope(Vector2D start, Vector2D end, int num_nodes, float node_mass, float k, vector<int> pinned_nodes)
    {
        // TODO (Part 1): Create a rope starting at `start`, ending at `end`, and containing `num_nodes` nodes.
        for (int i = 0; i < num_nodes; i++)
        {
            Vector2D temp = start + (end - start) * i/(num_nodes-1.0f);
            masses.push_back(new Mass(temp,node_mass,false));
            
        }
        for (int i = 0; i < num_nodes-1; i++)
        {
            springs.push_back(new Spring(masses[i],masses[i+1],k));
        }
        
//        Comment-in this part when you implement the constructor
        for (auto &i : pinned_nodes) {
            masses[i]->pinned = true;
        }
    }

    void Rope::simulateEuler(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            // TODO (Part 2): Use Hooke's law to calculate the force on a node
            float ks = s->k;
            Vector2D dis = s->m1->position - s->m2->position;
            s->m1->forces += -1 * ks * dis/dis.norm()*(dis.norm() - s->rest_length);
            s->m2->forces += -1 * ks * -dis/dis.norm()*(dis.norm() - s->rest_length);
        }

        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                // TODO (Part 2): Add the force due to gravity, then compute the new velocity and position
                m->forces += gravity * m->mass;
                Vector2D a = m->forces/m->mass;
                m->velocity += a * delta_t;
                m->position += m->velocity * delta_t;
                // TODO (Part 2): Add global damping
                m->last_position = m->position;
                m->velocity  *= (1-0.00005);
                
            }

            // Reset all forces on each mass
            m->forces = Vector2D(0, 0);
        }
    }

    void Rope::simulateVerlet(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            // TODO (Part 3): Simulate one timestep of the rope using explicit Verlet （solving constraints)
            Vector2D Direct = s->m2->position - s->m1->position;
            Vector2D recorrection = Direct * (Direct.norm() - s->rest_length)/ Direct.norm() / 2;      
            if (!s->m1->pinned)
            {
                s->m1->position += recorrection;
            }
            if (!s->m2->pinned)
            {
                s->m2->position += -recorrection;
            }
            
        }

        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                Vector2D temp_position = m->position;
                // TODO (Part 3.1): Set the new position of the rope mass
                // TODO (Part 4): Add global Verlet damping
                float damping = 0.00005;
                m->position += (1-damping) * (m->position -  m->last_position) + gravity * delta_t * delta_t;
                m->last_position = temp_position;
            }
        }
    }
}
