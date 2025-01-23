using System;
using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting.FullSerializer;
using UnityEngine;

[CreateAssetMenu(fileName = "BackflipAbilitySO", menuName = "ScriptableObject/BackflipAbility")]

// work in progress, untested - Jacob Lon 2/29
public class BackflipAbility : AbilityScriptableObject
{
    // used to increase player velocity during jump, similar to dash
    [SerializeField]
    private float jumpspeedscale = 20.0f; 

    // scaling how much the player goes up
    [SerializeField]
    private float jumpupscale = 5.0f;
    
    [SerializeField]
    private float dragValue = 0.01f; // increase drag on user body

    private GameObject user;
    private Rigidbody userBody;
    private Transform userT;
    private PlayerController uC;
    private GameObject flipVFX; // not in use currently
    
    // initialize ability data from caster data
    public override void Initialize(GameObject caster)
    {
        base.Initialize(caster);
        user = caster;
        userBody = user.GetComponent<Rigidbody>(); // use for velocity
        userT = user.GetComponent<Transform>(); // player transform data
        uC = user.GetComponent<PlayerController>();
        
        // look for vfx for flip
        var particle = caster.GetComponentInChildren<ParticleSystem>(false);
        if (particle)
        {
            flipVFX = particle.gameObject;
            if (flipVFX) flipVFX.SetActive(false);
        }
    }

    // Update is called once per frame
    public override void Update()
    {
        // immunity frames???
        
        //Console.Out.WriteLine("----Backflip----");
        
        base.Update();
        // increment drag, based on dash but we may not want for flip
        userBody.drag += dragValue;
        
        // on complete
        if (activeTime <= 0)
        {
            //Console.Out.WriteLine("----Backflip done----");
            // release player from ability 
            userBody.drag = 0;
            uC.setLockMovement(false);
            
            // quit effects
            if (flipVFX) flipVFX.SetActive(false);
        }
        else
        {
            
            Console.Out.WriteLine("----Backflip----");
        }
    }

    public override AbilityScriptableObject Activate(bool visual, int playerID,
        uint abilityIndex, bool empowered = false)
    {
        // check for ability base data
        if (base.Activate(visual, playerID, abilityIndex, empowered) == null)
            return null;
        
        // get up value and back value of player
        Vector3 upvector = userT.up * jumpupscale;
        Vector3 jumpvector = -userT.forward * jumpspeedscale;

        // this version increases jump speed by player speed
        // if player is moving when ability is activated
        // 2 versions we could do, either increase speed or manually increase distance
        //if (userBody.velocity.magnitude > 0.0f)
            //jumpvector = -userT.forward.normalized * (jumpspeedscale * uC.getSpeed());
        
        // move trajectory up
        //userBody.velocity = Vector3.Cross(jumpvector, upvector);

        userBody.velocity = jumpvector + upvector;
        
        // if we dont want to go up aka dash back
        //userBody.velocity = jumpvector;
        
        //userBody.velocity = -userT.forward.normalized * jumpspeedscale;
        uC.setLockMovement(true);
        
        // start flip visual effects
        if (flipVFX) flipVFX.SetActive(true);
        
        
        return this; // return reference to ability
    }
}
