using System.Collections;
using System.Collections.Generic;
using System.Numerics;
using Unity.VisualScripting;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.UI;

public class Spline : MonoBehaviour
{
    // Start is called before the first frame update

    public List<GameObject> knots;
    List<GameObject> curves;
    [SerializeField] private GameObject knot;
    [SerializeField] private GameObject curve;
    [SerializeField] private Button addCurveButton;
    [SerializeField] private Button removeCurveButton;

    private void Start()
    {
        curves = new List<GameObject>();
        knots = new List<GameObject>();

        GameObject startingKnot = Instantiate(knot, gameObject.transform.position, UnityEngine.Quaternion.identity);

        knots.Add(startingKnot);

        AddCurve();

        addCurveButton.onClick.AddListener(AddCurve);
        removeCurveButton.onClick.AddListener(RemoveCurve);
    }

    public void AddCurve()
    {
        GameObject newCurve = Instantiate(curve, gameObject.transform.position, UnityEngine.Quaternion.identity);
        curves.Add(newCurve);
        GameObject newKnot = Instantiate(knot, gameObject.transform.position, UnityEngine.Quaternion.identity);
        knots.Add(newKnot);
    }
    public void RemoveCurve()
    {
        if (curves.Count > 1)
        {
            Destroy(curves[^1]);
            Destroy(knots[^1]);
        }
    }
}
