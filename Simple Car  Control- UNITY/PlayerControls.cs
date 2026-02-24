// c# program using assembly:
using UnityEngine;
using UnityEngine.UIElements;

public class NewMonoBehaviourScript : MonoBehaviour
{
    // Private variables to control the speed and turn speed of the player
    private float speed = 28.0f;
    private float turnSpeed = 25.0f;
    private float horizontalInput;
    private float forwardInput;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        
    }

    // Update is called once per frame
 
    void Update()
    {
        // Player input for movement and turning
        horizontalInput = Input.GetAxis("Horizontal");
        forwardInput = Input.GetAxis("Vertical");
        // Move Forward 
        transform.Translate(Vector3.forward * Time.deltaTime * speed * forwardInput);
        // Turn the player left and right
        transform.Rotate(Vector3.up, turnSpeed * horizontalInput * Time.deltaTime);
    }
}
