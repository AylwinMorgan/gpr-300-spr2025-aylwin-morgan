using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class BezierCurve : MonoBehaviour
{

    public GameObject k1;
    public GameObject k2;
    [SerializeField] private Color color;
    private LineRenderer lineRenderer;
    const int subsegmentCount = 50;
    const float lineWidth = 0.05f;

    private void Start()
    {
        // instantiate knots
        getKnotsFromList();
        
        // setup linerenderer
        lineRenderer = gameObject.AddComponent<LineRenderer>();
        lineRenderer.positionCount = subsegmentCount;
        lineRenderer.startColor = color;
        lineRenderer.endColor = color;
        lineRenderer.startWidth = lineWidth;
        lineRenderer.endWidth = lineWidth;
    }

    public void getKnotsFromList()
    {
        GameObject spline = GameObject.FindGameObjectWithTag("Spline");
        k1 = spline.GetComponent<Spline>().knots[^2];
        k2 = spline.GetComponent<Spline>().knots[^1];
    }

    // Update is called once per frame
    void Update()
    {
        if (k1 != null && k2 != null)
        {
            for (int i = 0; i < subsegmentCount; i++)
            {
                lineRenderer.SetPosition(i, GetPositionAtTime((float)i / subsegmentCount));
            }
        }
    }

    public Vector3 GetPositionAtTime(float t)
    {
        Vector3 a = Vector3.Lerp(k1.transform.position,k1.GetComponent<Knot>().GetNextControlPoint(),t);
        Vector3 b = Vector3.Lerp(k1.GetComponent<Knot>().GetNextControlPoint(),k2.GetComponent<Knot>().GetPreviousControlPoint(),t);
        Vector3 c = Vector3.Lerp(k2.GetComponent<Knot>().GetPreviousControlPoint(),k2.transform.position,t);
        Vector3 d = Vector3.Lerp(a, b, t);
        Vector3 e = Vector3.Lerp(b, c, t);
        return Vector3.Lerp(d, e, t);
    }
}
