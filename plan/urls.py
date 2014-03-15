'''Url request dispatcher'''

from django.conf.urls import patterns, url
import views

urlpatterns = patterns('',
                       (r'^$', views.test),
                       (r'^changeTemp/(\d{1,2})/$', views.changeTemp),
		)
