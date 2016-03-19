using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Web.Http;

namespace WebApplication2.Controllers
{
    public class EspController : ApiController
    {

        // GET: api/Esp
        public IEnumerable<string> Get()
        {
            return new string[] { "value1", "value2" };
        }

        // GET: api/Esp/5?value=2
        public HttpResponseMessage Get(int id, [FromUri]int value)
        {
            /* If button 1 is updated */
            if (id == 1)
            {
                /* If value is changed */
                if (value != Database.inputs[1])
                {

                    if (value == 0)
                    {
                        WebRequest request = WebRequest.Create("https://maker.ifttt.com/trigger/prizda_off/with/key/bURJkfECQI1MIEuFurs6dg");
                        request.Method = "GET";
                        request.GetResponse();
                    }
                    else
                    {
                        WebRequest request = WebRequest.Create("https://maker.ifttt.com/trigger/prizda_on/with/key/bURJkfECQI1MIEuFurs6dg");
                        request.Method = "GET";
                        request.GetResponse();
                    }
                }

            }

            Database.inputs[id] = Convert.ToUInt16(value);

            /* Some smart connection matrix */
            Database.outputs[3] = Database.inputs[1];
            Database.outputs[2] = Database.inputs[1];
            Database.outputs[4] = Database.inputs[1];
            Database.outputs[1] = Database.inputs[1];

            return new HttpResponseMessage()
            {
                        Content = new StringContent(
                Convert.ToString(Database.outputs[id]),
                Encoding.UTF8,
                "text/html"
            )
                    };

        }


        // GET: api/Esp/5
        public HttpResponseMessage Get(int id)
        {
            return new HttpResponseMessage()
            {
                Content = new StringContent(
                    Convert.ToString(Database.outputs[id]),
                    Encoding.UTF8,
                    "text/html"
                )
            };
        }

        // POST: api/Esp
        [HttpPost]
        public void Post(int id,[FromUri]int value)
        {

            /* If button 1 is updated */
            if (id == 1)
            {
                /* If value is changed */
                if (value != Database.inputs[1])
                {
                    
                    if (value == 0)
                    {
                        WebRequest request = WebRequest.Create("https://maker.ifttt.com/trigger/prizda_off/with/key/bURJkfECQI1MIEuFurs6dg");
                        request.Method = "GET";
                        request.GetResponse();
                    }
                    else
                    {
                        WebRequest request = WebRequest.Create("https://maker.ifttt.com/trigger/prizda_on/with/key/bURJkfECQI1MIEuFurs6dg");
                        request.Method = "GET";
                        request.GetResponse();
                    }
                }
            }

            Database.inputs[id] = Convert.ToUInt16(value);

            /* Some smart connection matrix */
            Database.outputs[3] = Database.inputs[1];
            Database.outputs[2] = Database.inputs[1];
            Database.outputs[4] = Database.inputs[1];
            Database.outputs[1] = Database.inputs[1];


        }

        // PUT: api/Esp/5
        public void Put(int id, string value)
        {
        }

        // DELETE: api/Esp/5
        public void Delete(int id)
        {
        }
    }
}
