'use client'

import { useState, useEffect, type FormEvent, type ChangeEvent } from 'react'
import { createClient } from '@/lib/supabase'
import { MessageSquare, Calendar, MapPin, ExternalLink } from 'lucide-react'
import { format } from 'date-fns'

interface Profile {
  full_name: string | null
  campus: string | null
}

interface Channel {
  id: string
  name: string
  description: string | null
  campus: string | null
  type: 'campus' | 'class' | 'interest'
  created_at: string
}

interface Post {
  id: string
  channel_id: string
  author_id: string
  title: string
  content: string
  resource_url: string | null
  upvotes: number
  created_at: string
  profiles?: Profile | null
}

interface SessionRsvp {
  user_id: string
  status: 'going' | 'maybe'
}

interface StudySession {
  id: string
  channel_id: string
  creator_id: string
  title: string
  description: string | null
  location_type: 'in-person' | 'virtual'
  location_detail: string
  start_time: string
  created_at: string
  profiles?: { full_name: string | null } | null
  session_rsvps?: SessionRsvp[]
}

export default function Dashboard() {
  const supabase = createClient()

  const [channels, setChannels] = useState<Channel[]>([])
  const [activeChannel, setActiveChannel] = useState<Channel | null>(null)
  const [posts, setPosts] = useState<Post[]>([])
  const [sessions, setSessions] = useState<StudySession[]>([])
  const [activeTab, setActiveTab] = useState<'threads' | 'sessions'>('threads')

  const [newPostTitle, setNewPostTitle] = useState('')
  const [newPostContent, setNewPostContent] = useState('')
  const [newResourceUrl, setNewResourceUrl] = useState('')

  const [sessionTitle, setSessionTitle] = useState('')
  const [sessionType, setSessionType] = useState<'in-person' | 'virtual'>('in-person')
  const [sessionDetail, setSessionDetail] = useState('')
  const [sessionDate, setSessionDate] = useState('')

  // 1. Fetch channels on initial mount
  useEffect(() => {
    let isMounted = true

    async function fetchChannels() {
      const { data } = await supabase.from('channels').select('*')
      if (isMounted && data && data.length > 0) {
        const channelList = data as Channel[]
        setChannels(channelList)
        setActiveChannel((prev) => prev ?? channelList[0])
      }
    }

    fetchChannels()

    return () => {
      isMounted = false
    }
  }, [supabase])

  // 2. Fetch feed when activeChannel changes (with explicit channelId narrowing)
  useEffect(() => {
    if (!activeChannel) return
    const channelId = activeChannel.id
    let isMounted = true

    async function fetchFeed() {
      const [postsRes, sessionsRes] = await Promise.all([
        supabase
          .from('posts')
          .select('*, profiles(full_name, campus)')
          .eq('channel_id', channelId)
          .order('created_at', { ascending: false }),
        supabase
          .from('study_sessions')
          .select('*, profiles(full_name), session_rsvps(user_id, status)')
          .eq('channel_id', channelId)
          .order('start_time', { ascending: true }),
      ])

      if (isMounted) {
        setPosts((postsRes.data as Post[]) || [])
        setSessions((sessionsRes.data as StudySession[]) || [])
      }
    }

    fetchFeed()

    return () => {
      isMounted = false
    }
  }, [activeChannel, supabase])

  // Helper to refresh feed after creating a post or session
  async function refreshFeed(channelId: string) {
    const [postsRes, sessionsRes] = await Promise.all([
      supabase
        .from('posts')
        .select('*, profiles(full_name, campus)')
        .eq('channel_id', channelId)
        .order('created_at', { ascending: false }),
      supabase
        .from('study_sessions')
        .select('*, profiles(full_name), session_rsvps(user_id, status)')
        .eq('channel_id', channelId)
        .order('start_time', { ascending: true }),
    ])

    setPosts((postsRes.data as Post[]) || [])
    setSessions((sessionsRes.data as StudySession[]) || [])
  }

  async function handleCreatePost(e: FormEvent<HTMLFormElement>) {
    e.preventDefault()
    const { data: { user } } = await supabase.auth.getUser()
    if (!user) return alert('Log in with your CUNY email')
    if (!activeChannel) return

    await supabase.from('posts').insert({
      channel_id: activeChannel.id,
      author_id: user.id,
      title: newPostTitle,
      content: newPostContent,
      resource_url: newResourceUrl || null,
    })

    setNewPostTitle('')
    setNewPostContent('')
    setNewResourceUrl('')
    refreshFeed(activeChannel.id)
  }

  async function handleCreateSession(e: FormEvent<HTMLFormElement>) {
    e.preventDefault()
    const { data: { user } } = await supabase.auth.getUser()
    if (!user) return alert('Log in with your CUNY email')
    if (!activeChannel) return

    await supabase.from('study_sessions').insert({
      channel_id: activeChannel.id,
      creator_id: user.id,
      title: sessionTitle,
      location_type: sessionType,
      location_detail: sessionDetail,
      start_time: new Date(sessionDate).toISOString(),
    })

    setSessionTitle('')
    setSessionDetail('')
    setSessionDate('')
    refreshFeed(activeChannel.id)
  }

  return (
    <div className="flex h-screen bg-slate-950 text-slate-100">
      <aside className="w-64 border-r border-slate-800 bg-slate-900/50 flex flex-col p-4">
        <div className="flex items-center gap-2 mb-6">
          <div className="bg-blue-600 p-1.5 rounded-lg font-bold">CV</div>
          <span className="font-bold text-lg tracking-tight">CUNYVerse</span>
        </div>

        <div className="flex-1 overflow-y-auto space-y-4">
          <div>
            <div className="text-xs font-semibold text-slate-400 uppercase tracking-wider mb-2">
              Class & Campus Channels
            </div>
            <div className="space-y-1">
              {channels.map((ch) => (
                <button
                  type="button"
                  key={ch.id}
                  onClick={() => setActiveChannel(ch)}
                  className={`w-full text-left px-2.5 py-1.5 rounded text-sm flex items-center gap-2 transition ${
                    activeChannel?.id === ch.id
                      ? 'bg-blue-600 text-white font-medium'
                      : 'text-slate-400 hover:bg-slate-800 hover:text-slate-200'
                  }`}
                >
                  <span className="text-slate-500">#</span>
                  <span className="truncate">{ch.name}</span>
                </button>
              ))}
            </div>
          </div>
        </div>

        <div className="border-t border-slate-800 pt-3 text-xs text-slate-400">
          Logged in as CUNY Student
        </div>
      </aside>

      <main className="flex-1 flex flex-col min-w-0">
        <header className="h-14 border-b border-slate-800 px-6 flex items-center justify-between">
          <div className="flex items-center gap-2 font-semibold">
            <span>#{activeChannel?.name || 'select-channel'}</span>
            <span className="text-xs text-slate-400 font-normal border border-slate-700 px-2 py-0.5 rounded">
              {activeChannel?.campus || 'All CUNY'}
            </span>
          </div>
          <div className="flex gap-2">
            <button
              type="button"
              onClick={() => setActiveTab('threads')}
              className={`px-3 py-1.5 rounded text-sm flex items-center gap-1.5 ${
                activeTab === 'threads' ? 'bg-slate-800 text-white' : 'text-slate-400'
              }`}
            >
              <MessageSquare className="w-4 h-4" /> Discussion Threads
            </button>
            <button
              type="button"
              onClick={() => setActiveTab('sessions')}
              className={`px-3 py-1.5 rounded text-sm flex items-center gap-1.5 ${
                activeTab === 'sessions' ? 'bg-slate-800 text-white' : 'text-slate-400'
              }`}
            >
              <Calendar className="w-4 h-4" /> Study Sessions
            </button>
          </div>
        </header>

        <div className="flex-1 overflow-y-auto p-6 max-w-4xl w-full mx-auto space-y-6">
          {activeTab === 'threads' ? (
            <>
              <form onSubmit={handleCreatePost} className="bg-slate-900 border border-slate-800 rounded-xl p-4 space-y-3">
                <input
                  type="text"
                  placeholder="Post Title (e.g. Midterm 1 Study Guide for CS 211)"
                  value={newPostTitle}
                  onChange={(e) => setNewPostTitle(e.target.value)}
                  className="w-full bg-slate-950 border border-slate-800 rounded px-3 py-1.5 text-sm outline-none focus:border-blue-500"
                  required
                />
                <textarea
                  placeholder="Share your questions, notes, or tips..."
                  value={newPostContent}
                  onChange={(e) => setNewPostContent(e.target.value)}
                  className="w-full bg-slate-950 border border-slate-800 rounded px-3 py-2 text-sm outline-none focus:border-blue-500 min-h-[80px]"
                  required
                />
                <div className="flex gap-2">
                  <input
                    type="url"
                    placeholder="Link to Google Doc / Notion (optional)"
                    value={newResourceUrl}
                    onChange={(e) => setNewResourceUrl(e.target.value)}
                    className="flex-1 bg-slate-950 border border-slate-800 rounded px-3 py-1.5 text-xs outline-none"
                  />
                  <button type="submit" className="bg-blue-600 hover:bg-blue-700 text-white px-4 py-1.5 rounded text-sm font-medium">
                    Post Thread
                  </button>
                </div>
              </form>

              <div className="space-y-3">
                {posts.map((post) => (
                  <article key={post.id} className="bg-slate-900/60 border border-slate-800/80 rounded-xl p-4 hover:border-slate-700 transition">
                    <div className="flex items-center gap-2 text-xs text-slate-400 mb-2">
                      <span className="font-semibold text-slate-200">{post.profiles?.full_name || 'Anonymous'}</span>
                      <span>•</span>
                      <span>{post.profiles?.campus || 'CUNY'}</span>
                      <span>•</span>
                      <span>{format(new Date(post.created_at), 'MMM d, h:mm a')}</span>
                    </div>
                    <h3 className="font-semibold text-base mb-1 text-slate-100">{post.title}</h3>
                    <p className="text-sm text-slate-300 whitespace-pre-line mb-3">{post.content}</p>
                    {post.resource_url && (
                      <a
                        href={post.resource_url}
                        target="_blank"
                        rel="noreferrer"
                        className="inline-flex items-center gap-1.5 text-xs text-blue-400 bg-blue-950/40 border border-blue-800 px-2.5 py-1 rounded"
                      >
                        <ExternalLink className="w-3.5 h-3.5" /> Attached Study Resource
                      </a>
                    )}
                  </article>
                ))}
              </div>
            </>
          ) : (
            <>
              <form onSubmit={handleCreateSession} className="bg-slate-900 border border-slate-800 rounded-xl p-4 space-y-3">
                <h4 className="text-sm font-semibold">Propose a Study Session</h4>
                <div className="grid grid-cols-2 gap-3">
                  <input
                    type="text"
                    placeholder="Session Topic (e.g. Calculus Quiz 3 Cram)"
                    value={sessionTitle}
                    onChange={(e) => setSessionTitle(e.target.value)}
                    className="bg-slate-950 border border-slate-800 rounded px-3 py-1.5 text-sm outline-none"
                    required
                  />
                  <input
                    type="datetime-local"
                    value={sessionDate}
                    onChange={(e) => setSessionDate(e.target.value)}
                    className="bg-slate-950 border border-slate-800 rounded px-3 py-1.5 text-sm outline-none text-slate-300"
                    required
                  />
                </div>
                <div className="grid grid-cols-2 gap-3">
                  <select
                    value={sessionType}
                    onChange={(e: ChangeEvent<HTMLSelectElement>) => setSessionType(e.target.value as 'in-person' | 'virtual')}
                    className="bg-slate-950 border border-slate-800 rounded px-3 py-1.5 text-sm outline-none"
                  >
                    <option value="in-person">In-Person (Campus Library / Lounge)</option>
                    <option value="virtual">Virtual (Zoom / Discord)</option>
                  </select>
                  <input
                    type="text"
                    placeholder={sessionType === 'in-person' ? 'Hunter Library 4th Floor' : 'https://zoom.us/j/...'}
                    value={sessionDetail}
                    onChange={(e) => setSessionDetail(e.target.value)}
                    className="bg-slate-950 border border-slate-800 rounded px-3 py-1.5 text-sm outline-none"
                    required
                  />
                </div>
                <button type="submit" className="w-full bg-blue-600 hover:bg-blue-700 text-white py-1.5 rounded text-sm font-medium">
                  Schedule Meetup
                </button>
              </form>

              <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                {sessions.map((session) => (
                  <div key={session.id} className="bg-slate-900 border border-slate-800 rounded-xl p-4 flex flex-col justify-between">
                    <div>
                      <div className="flex items-center justify-between text-xs text-blue-400 font-semibold mb-1">
                        <span>{format(new Date(session.start_time), 'EEE, MMM d • h:mm a')}</span>
                        <span className="uppercase text-[10px] bg-slate-800 px-1.5 py-0.5 rounded text-slate-300">
                          {session.location_type}
                        </span>
                      </div>
                      <h4 className="font-semibold text-base mb-1">{session.title}</h4>
                      <p className="text-xs text-slate-400 flex items-center gap-1 mb-3">
                        <MapPin className="w-3.5 h-3.5 text-slate-500" />
                        {session.location_detail}
                      </p>
                    </div>
                    <div className="border-t border-slate-800/80 pt-3 flex items-center justify-between">
                      <span className="text-xs text-slate-400">
                        {session.session_rsvps?.length || 0} students going
                      </span>
                      <button type="button" className="bg-slate-800 hover:bg-blue-600 text-slate-200 hover:text-white px-3 py-1 rounded text-xs transition">
                        RSVP
                      </button>
                    </div>
                  </div>
                ))}
              </div>
            </>
          )}
        </div>
      </main>
    </div>
  )
}