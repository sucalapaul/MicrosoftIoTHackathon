using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;

namespace WebApplication2.Controllers
{
    public class HomeController : Controller
    {
        public ActionResult Index()
        {
            return View();
        }

        public ActionResult About()
        {
            ViewBag.Message = "Your application description page.";

            return View();
        }

        public ActionResult Contact()
        {
            ViewBag.Message = "Your contact page.";

            return View();
        }

        public ActionResult Device()
        {
            ViewBag.Message = "Devices";
            ViewBag.direction = Database.direction;
            ViewBag.inputs = Database.inputs;
            ViewBag.outputs = Database.outputs;

            return View();
        }


        public ActionResult DeviceAdd(int id)
        {
            ViewBag.Message = "Devices";
            ViewBag.DeviceId = id;
            Database.direction[id] = 2;
            return View();
        }
    }
}