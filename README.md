## Fomalhaut
#### A clean, simple, c++11, Entity Component System like Overwatch ECS architecture

## Overview
This is a simple entity component system library based on the [Overwatch Gameplay Architecture and Netcode](http://gad.qq.com/article/detail/28682) article. It is written in c++11 without other dependencies. You can easily add include-folder into your game project.
##### NOTE:
This is my thought about how the 'Overwatch' ECS should look. If you want to add it to a commercial project, you'd better read the source code and master it. Then add the features that you need.
## Support
* gcc >= 4.8 (tested with v4.8.4)
* clang
* MSVC >= VS2015

## Installation
1. Git pull source code
2. cd Fomalhaut && mkdir build
3. Run ./run.sh

## Tutorial

#### Create An EntityAdmin
EntityAdmin controlls all entities and systems. You can simply define it by the following code:
<pre><code>EntityAdmin admin; </code></pre>    

#### Create Component
Declare some components that derived from BaseComponent such as:
<pre><code>class MovementComponent : public BaseComponent
{
public:
    void Reset(float velocity) { this->velocity = velocity; }
    void Print() { std::cout << "MovementComponent: " << velocity << std::endl; }
    float velocity;
};
class HealthComponent : public BaseComponent
{
public:
    void Reset(float hp, float mana)
    {
        this->hp = hp;
        this->mana = mana;
    }
    void Print() { std::cout << "HealthComponent: " << hp << '\t' << mana << std::endl; }
    float hp, mana;
};
class PositionComponent : public BaseComponent
{
public:
    void Reset(float px, float py, float pz)
    {
        x = px;
        y = py;
        z = pz;
    }
    void Print() { std::cout << "PositionComponent: " << x << '\t' << y << '\t' << z << std::endl; }
    float x, y, z;
};
</code></pre>

#### Create An Entity
Entity is an aggregate that consists of one or more components. It was created by An EntityAdmin.
<pre><code>Entity& entity = admin.CreateEntity&lt;Entity>();</code></pre>
You can simply add or replace component by:
<pre><code>entity.Add&lt;PositionComponent>(3.f, 7.f, 10.f);
entity.Replace&lt;PositionComponent>(10.f, 100.f, 100.f);
</code></pre>
To check whether entity  has some components:
<pre><code>bool result = entity.Has&lt;HealthComponent>();
bool result = entity.Has&lt;PositionComponent, HealthComponent>();
</code></pre>
Remove one or more components:
<pre><code>entity.Remove&lt;PositionComponent>();
entity.Remove&lt;PositionComponent, HealthComponent>();
</code></pre>
Use tuple to get some componets one-time:
<pre><code>PositionComponent* p = entity.Get&lt;PositionComponent>();
std::tuple&lt;PositionComponent*, HealthComponent*> va1 = entity.Get&lt;PositionComponent, HealthComponent>();
</code></pre>

#### Create A Demosystem And Iterate Components
<pre><code>class DemoSystem : public BaseSystem
{
public:
    using BaseSystem::BaseSystem;
    void Update(float time_step) override
    {
        for (PositionComponent* p : ComponentItr&lt;PositionComponent>(admin_)) {
            p->Print();
            HealthComponent* h = p->Sibling&lt;HealthComponent>();
            if (h) { h->Print(); }
            MovementComponent* m = p->Sibling&lt;MovementComponent>();
            if (m) { m->Print(); }
        }
    }
};
DemoSystem& sys = admin.CreateSystem&lt;DemoSystem>();
sys.Update(0.1f);
</code></pre>
Use tuple-iterator to iterate Entity that has specific components with specific condition:
<pre><code>for (std::tuple&lt;PositionComponent*, HealthComponent*>&& t : TupleItr&lt;PositionComponent, HealthComponent>(
             &admin, [](const PositionComponent*p, const HealthComponent* h) -> bool { return h->hp > 60; })) 
{
    std::get&lt;0>(t)->Print();
    std::get&lt;1>(t)->Print();
}</code></pre>