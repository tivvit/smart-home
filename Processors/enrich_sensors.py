'''
runned via cron every minute
'''

from elasticsearch import Elasticsearch

# es = Elasticsearch(hosts=['j.tivvit.cz:9200'])
es = Elasticsearch(hosts=['10.0.0.1:9200'])

res = es.search(index="logstash-*", doc_type="sensor", body={
    "query": {
        "range": {
            "@timestamp": {
                "gt": "now-2m"
            }
        }
    }
}, size="10000")

for i in res["hits"]["hits"]:
    # print i
    sensor_id = i['_source']['place'] + "_" + i['_source']['location'] + "_" + \
                i['_source']['variable']
    es.update(index=i["_index"], doc_type=i["_type"], id=i["_id"], refresh=True,
              body={'doc': {"sensor_id": sensor_id}})
