#ifndef OPENMW_COMPONENTS_NIFOSG_PARTICLE_H
#define OPENMW_COMPONENTS_NIFOSG_PARTICLE_H

#include <osgParticle/Particle>
#include <osgParticle/Shooter>
#include <osgParticle/Operator>

#include <osg/NodeCallback>

#include <components/nif/nifkey.hpp>
#include <components/nif/data.hpp>

#include "controller.hpp" // ValueInterpolator

namespace Nif
{
    class NiGravity;
}

namespace NifOsg
{

    // Subclass ParticleSystem to support a limit on the number of active particles.
    class ParticleSystem : public osgParticle::ParticleSystem
    {
    public:
        ParticleSystem();
        ParticleSystem(const ParticleSystem& copy, const osg::CopyOp& copyop);

        META_Object(NifOsg, NifOsg::ParticleSystem)

        virtual osgParticle::Particle* createParticle(const osgParticle::Particle *ptemplate);

        void setQuota(int quota);

    private:
        int mQuota;
    };

    // HACK: Particle doesn't allow setting the initial age, but we need this for loading the particle system state
    class ParticleAgeSetter : public osgParticle::Particle
    {
    public:
        ParticleAgeSetter(float age)
            : Particle()
        {
            _t0 = age;
        }
    };

    // Node callback used to set the inverse of the parent's world matrix on the MatrixTransform
    // that the callback is attached to. Used for certain particle systems,
    // so that the particles do not move with the node they are attached to.
    class InverseWorldMatrix : public osg::NodeCallback
    {
    public:
        InverseWorldMatrix()
        {
        }
        InverseWorldMatrix(const InverseWorldMatrix& copy, const osg::CopyOp& op = osg::CopyOp::SHALLOW_COPY)
            : osg::NodeCallback(), osg::Object()
        {
        }

        META_Object(NifOsg, InverseWorldMatrix)

        void operator()(osg::Node* node, osg::NodeVisitor* nv);
    };

    class ParticleShooter : public osgParticle::Shooter
    {
    public:
        ParticleShooter(float minSpeed, float maxSpeed, float horizontalDir, float horizontalAngle, float verticalDir, float verticalAngle,
                        float lifetime, float lifetimeRandom);
        ParticleShooter();
        ParticleShooter(const Shooter& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

        META_Object(NifOsg, ParticleShooter)

        virtual void shoot(osgParticle::Particle* particle) const;

    private:
        float mMinSpeed;
        float mMaxSpeed;
        float mHorizontalDir;
        float mHorizontalAngle;
        float mVerticalDir;
        float mVerticalAngle;
        float mLifetime;
        float mLifetimeRandom;
    };

    class GrowFadeAffector : public osgParticle::Operator
    {
    public:
        GrowFadeAffector(float growTime, float fadeTime);
        GrowFadeAffector();
        GrowFadeAffector(const GrowFadeAffector& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

        META_Object(NifOsg, GrowFadeAffector)

        virtual void beginOperate(osgParticle::Program* program);
        virtual void operate(osgParticle::Particle* particle, double dt);

    private:
        float mGrowTime;
        float mFadeTime;

        float mCachedDefaultSize;
    };

    class ParticleColorAffector : public osgParticle::Operator, public ValueInterpolator
    {
    public:
        ParticleColorAffector(const Nif::NiColorData* clrdata);
        ParticleColorAffector();
        ParticleColorAffector(const ParticleColorAffector& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

        META_Object(NifOsg, ParticleColorAffector)

        // TODO: very similar to vec3 version, refactor to a template
        osg::Vec4f interpolate(const float time, const Nif::Vector4KeyMap::MapType& keys);

        virtual void operate(osgParticle::Particle* particle, double dt);

    private:
        Nif::NiColorData mData;
    };

    class GravityAffector : public osgParticle::Operator
    {
    public:
        GravityAffector(const Nif::NiGravity* gravity);
        GravityAffector();
        GravityAffector(const GravityAffector& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

        META_Object(NifOsg, GravityAffector)

        virtual void operate(osgParticle::Particle* particle, double dt);
        virtual void beginOperate(osgParticle::Program *);

    private:
        float mForce;
        enum ForceType {
            Type_Wind,
            Type_Point
        };
        ForceType mType;
        osg::Vec3f mPosition;
        osg::Vec3f mDirection;
        osg::Vec3f mCachedWorldPositionDirection;
    };

}

#endif