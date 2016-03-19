using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Web.Http;

namespace WebApplication2.Controllers
{
    public class Esp_ConfigController : ApiController
    {
        // GET: api/Esp_Config
        public IEnumerable<string> Get()
        {
            return new string[] { "value1", "value2" };
        }

        // GET: api/Esp_Config/5
        public HttpResponseMessage Get(int id)
        {
            return new HttpResponseMessage()
            {
                Content = new StringContent(
                    Convert.ToString(Database.direction[id]),
                    Encoding.UTF8,
                    "text/html"
                )
            };
        }

        
        // POST: api/Esp_Config
        [HttpPost]
        public void Post(int id, [FromUri]int value)
        {
            Database.direction[id] = Convert.ToUInt16(value);
        }

        // PUT: api/Esp_Config/5
        public void Put(int id, [FromBody]string value)
        {
        }

        // DELETE: api/Esp_Config/5
        public void Delete(int id)
        {
        }
    }
}
