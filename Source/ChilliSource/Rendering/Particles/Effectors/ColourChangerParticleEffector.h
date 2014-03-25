//
//  ColourChangerParticleEffector.h
//  moFloTest
//
//  Created by Scott Downie on 11/04/2011.
//  Copyright 2011 Tag Games. All rights reserved.
//


#ifndef _MO_FLO_RENDERING_PARTICLES_EFFECTORS_COLOUR_CHANGER_PARTICLE_EFFECTOR_H_
#define _MO_FLO_RENDERING_PARTICLES_EFFECTORS_COLOUR_CHANGER_PARTICLE_EFFECTOR_H_

#include <ChilliSource/ChilliSource.h>
#include <ChilliSource/Rendering/Particles/Effectors/ParticleEffector.h>
#include <ChilliSource/Core/Container/ParamDictionary.h>

namespace ChilliSource
{
    namespace Rendering
    {
        class ColourChangerParticleEffector : public ParticleEffector
        {
        public:
            ColourChangerParticleEffector(const Core::ParamDictionary& insParams);
            //-----------------------------------------------------
            /// Create
            ///
            /// Create a new effector from the given parameters
            ///
            /// @param Param dictionary
            /// @return Ownership of effector
            //-----------------------------------------------------
            static ParticleEffectorUPtr Create(const Core::ParamDictionary& inParams);
            //-----------------------------------------------------
            /// Init
            ///
            /// The effector will initialise the particles to its
            /// liking 
            ///
            /// @param Particle to intialise
            //-----------------------------------------------------
            void Init(Particle& in_particle);
            //-----------------------------------------------------
            /// Apply
            ///
            /// The effector will apply itself to the given 
            /// particles
            ///
            /// @param Particle to effect
            /// @param Time between frames
            //-----------------------------------------------------
            void Apply(Particle& in_particle, f32 infDt);
            
        private:
            
            bool mbInitialColourSet;
            Core::Colour mInitialColour;
            Core::Colour mTargetColour;
        };
    }
}

#endif