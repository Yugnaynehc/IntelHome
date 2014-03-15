from django.shortcuts import render_to_response
from django.http import HttpResponse
import time

def test(request):
    temp = getTemp();
    alert = 0
    
    if temp > 50:
        alert = 1
    else:
        alert = 0

    return render_to_response("plan/demo.html", {'string': 'Kevin', 'alert': alert, 'temp': temp})

def getTemp():
    with open('temp') as f:
        return int(f.readline())

def changeTemp(request, value):
    #try:
    #    value = int(value)
    #except ValueError:
    #    raise Http404()
    fp = open("temp", "w")
    fp.write(value)
    fp.close()
    return HttpResponse(value)
